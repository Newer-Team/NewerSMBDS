# Anything specific to compiling in C++.


import hashlib
import os, os.path
import shutil
import tempfile
import time

import common


# We need to put these around header files that only refer to symbols
# that may be compiled previously or in the future. Otherwise, we get
# "not defined" errors, and it won't link correctly.
EXTERN_H_PREPEND = """
#ifdef __cplusplus
extern "C"
{
#endif
"""
EXTERN_H_APPEND = """
#ifdef __cplusplus
}
#endif
"""


HEADER_EXTS = ['.h', '.hpp']
CODE_EXTS = ['.c', '.cpp', '.s']



def compileUnit(project, codeUnit, metaFiles):
    """
    Run Make for a particular CodeUnit.
    Return the contents of newcode.bin and newcode.sym as a bytes and a
    string, respectively.
    """
    
    with tempfile.TemporaryDirectory() as tmpdirname:
        if ' ' in tmpdirname:
            raise RuntimeError(
                'Temporary directory "{tmpdirname}" has spaces in it!! D:')

        # Prepare this as the compile directory

        def copy(src, dst):
            shutil.copyfile(os.path.join(*src), os.path.join(*dst))

        copy([os.path.dirname(__file__), 'buildtools', 'cpp', 'linker.x'],
             [tmpdirname, 'linker.x'])
        copy([os.path.dirname(__file__), 'buildtools', 'cpp', 'Makefile'],
             [tmpdirname, 'Makefile'])

        os.makedirs(os.path.join(tmpdirname, 'source'))
        for fn in codeUnit.sourcePaths:
            dst = [tmpdirname, 'source', os.path.basename(fn)]
            copy([fn], dst)
            timeInThePast = (2013, 6, 8, 0, 0, 0, 5, 159, -1)
            common.setModifiedTime(os.path.join(*dst), timeInThePast)

        os.makedirs(os.path.join(tmpdirname, 'build'))
        if project.useCache:
            for trueFn, destFn in queryCache(codeUnit):
                copy([trueFn], [tmpdirname, 'build', destFn])
                common.setModifiedTime(os.path.join(*dst), time.gmtime())

        for hn, hf in metaFiles.headers.items():
            with open(os.path.join(tmpdirname, 'source', hn),
                    'w', encoding='utf-8') as f:
                f.write(hf)
        copy([project.file('game.h')], [tmpdirname, 'source', 'game.h'])

        with open(os.path.join(tmpdirname, 'symbols.x'),
                'w', encoding='utf-8') as f:
            f.write('\n'.join(str(s) for s in metaFiles.symbols))

        # Call make
        if codeUnit.codeAddress is None:
            raise ValueError("Code address is None!! Something's messed up...")
        with common.cwdAs(tmpdirname):
            if os.system('make CODEADDR=0x%08X' % codeUnit.codeAddress) != 0:
                raise RuntimeError('Compilation failed. :(')

        # Grab things

        codefp = os.path.join(tmpdirname, 'newcode.bin')
        if not os.path.isfile(codefp):
            raise RuntimeError('newcode.bin does not exist! :(')
        with open(codefp, 'rb') as f:
            code = f.read()

        if codeUnit.maxLength is not None:
            if len(code) > codeUnit.maxLength:
                raise RuntimeError(f'Code is too large! '
                    f'({hex(len(code))} > {hex(codeUnit.maxLength)}) D:')

        codeUnit.reportSpaceUsed(len(code))

        symfp = os.path.join(tmpdirname, 'newcode.sym')
        if not os.path.isfile(symfp):
            raise RuntimeError('newcode.sym does not exist! :(')
        with open(symfp, 'r', encoding='utf-8') as f:
            sym = f.read()

        objectFiles = {}
        for fn in os.listdir(os.path.join(tmpdirname, 'build')):
            if fn.endswith('.o'):
                genericFilename = '.'.join(fn.split('.')[:-1])
                actualFilename = os.path.join(tmpdirname, 'build', fn)
                with open(actualFilename, 'rb') as f:
                    objectFiles[genericFilename] = f.read()

        updateCache(codeUnit, objectFiles)

        return code, sym


def updateCache(codeUnit, objectFiles):
    """
    Save the object files produced by make (along with the
    hashes of the corresponding source code files) to be used to
    speed up future compilations.
    """
    if not objectFiles:
        print('WARNING: No object files found. Your makefile probably'
            ' doesn\'t mark them as "secondary". Add:\n'
            '.SECONDARY: $(OFILES)\n'
            'after the line:\n'
            'all: $(OUTPUT).bin $(OUTPUT).sym\n'
            'in buildtools/Makefile.')
        return

    cacheDir = os.path.join(codeUnit.path, 'cache')

    # Make sure to delete the existing object files, lest they
    # build up and take over your hard drive
    if os.path.isdir(cacheDir):
        shutil.rmtree(cacheDir, ignore_errors=True)
    os.mkdir(cacheDir)

    for niceFilename, objData in objectFiles.items():
        # Try to find the corresponding code filename
        for codeFilename in codeUnit.pathsEndingWith(CODE_EXTS):
            codeBasename = os.path.basename(codeFilename)
            niceCodeFN = '.'.join(codeBasename.split('.')[:-1])
            if niceFilename == niceCodeFN: break
        else:
            continue

        with open(codeFilename, 'rb') as f:
            sourceCode = f.read()

        hash = hashlib.sha3_256(sourceCode).hexdigest()

        objectFn = f'{hash}_{niceFilename}.o'

        with open(os.path.join(cacheDir, objectFn), 'wb') as f:
            f.write(objData)


def queryCache(codeUnit):
    """
    Prepare a list of cached object files matching source files
    that haven't been modified since the last compile.
    Returns: [(trueFilename, destFilename), ...]
    where trueFilename is the full path to the cached object
    file, and destFilename is what it should be saved as in the
    build directory.
    """
    if not os.path.isdir(os.path.join(codeUnit.path, 'cache')):
        return []

    # First, hash all the existing source files
    hashToNames = {}
    for codeFilename in codeUnit.pathsEndingWith(CODE_EXTS):
        codeBasename = os.path.basename(codeFilename)
        niceCodeFN = '.'.join(codeBasename.split('.')[:-1])

        with open(codeFilename, 'rb') as f:
            sourceCode = f.read()

        hash = hashlib.sha3_256(sourceCode).hexdigest()

        hashToNames[hash] = niceCodeFN

    # Next, go through the cache folder and match those hashes
    # with the ones in the source files
    result = []
    for f in os.listdir(os.path.join(codeUnit.path, 'cache')):
        if len(f) < 64: continue
        hash = f[:64]

        if hash not in hashToNames: continue
        name = hashToNames[hash]

        trueFilename = os.path.join(codeUnit.path, 'cache', f)
        destFilename = name + '.o'
        result.append((trueFilename, destFilename))

    return result


def exportsFile(codeUnit):
    """
    The contents of exports.h, or None if it doesn't exist.
    """
    if not hasattr(codeUnit, '_exportsH'):
        fp = os.path.join(codeUnit.path, 'exports.h')
        if os.path.isfile(fp):
            with open(fp, 'r', encoding='utf-8') as f:
                codeUnit._exportsH = f.read()
        else:
            codeUnit._exportsH = None
    return codeUnit._exportsH


def combineExports(codeUnits):
    """
    Create a single .h file for all of the functions exported from the
    given code units.
    """
    head = [EXTERN_H_PREPEND]
    for inv in codeUnits:
        head.append(exportsFile(inv))
    head.append(EXTERN_H_APPEND)
    return 'overlays.h', '\n'.join(head)


def iterHeaderFiles(codeUnit):
    """
    Iterate over all header files this code unit uses.
    """
    for hp in codeUnit.pathsEndingWith(HEADER_EXTS):
        with open(hp, 'r', encoding='utf-8') as f:
            hd = f.read()
        yield os.path.basename(hp), '\n'.join([EXTERN_H_PREPEND, hd, EXTERN_H_APPEND])