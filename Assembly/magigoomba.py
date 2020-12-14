# Magigoomba: a cross-platform compiler for Nintendo DS game code
# patches that supports more features than NSMB Editor and isn't hacks
# layered upon hacks.
# By RoadrunnerWMC


import argparse
import collections
from datetime import datetime
import getpass
import json
import os, os.path
import shutil
import struct
import time

import ndspy
import ndspy.code
import ndspy.fnt
import ndspy.rom

import code_cpp



def makeBranchOpcode(srcAddr, destAddr, withLink):
    """
    Make an ARM branch opcode from srcAddr to destAddr, optionally with
    a link.
    """
    res = 0xEA000000

    if withLink:
        res |= 0x01000000

    offs = (destAddr // 4) - (srcAddr // 4) - 2
    offs &= 0x00FFFFFF
    res |= offs

    return res


def rebaseInstructionAtAddress(instruction, oldAddress, newAddress):
    """
    Adjust the instruction given (as an integer) if needed, so that it
    will work the same if moved to newAddress.
    """
    if instruction & 0x0E000000 == 0x0A000000: # Branch instruction
        offs = instruction & 0x00FFFFFF

        # Handle the case where that represents a negative offset
        if offs & 0x00800000:
            offs = struct.unpack('>i', struct.pack('>I', 0xFF000000 | offs))[0]

        # "dest" and "offs" are measured in instructions rather than
        # bytes; that is, they're multiples of 4 bytes. (Hence the
        # "// 4"s.)
        dest = offs + 2 + oldAddress // 4
        offs = dest - (newAddress // 4) - 2

        instruction = instruction & 0xFF000000 | offs & 0x00FFFFFF

    return instruction


def fmtAddr(addr, include_0x=True):
    """
    A little helper function to write an address as a string nicely.
    "0x" will be included only if include_0x is True (the default).
    """
    return ('0x' if include_0x else '') + ('%08X' % addr)


class Project:
    """
    Provides a nice API to access the files of a project.
    """

    class CodeUnit:
        """
        A class that represents one full, contiguous unit of code to be
        compiled.
        More specifically, this represents one of the folders in the
        project folder, such as an "insert", "append" or "overlay"
        folder.
        This isn't specific to any particular language.
        """
        ramAddress = 0x00000000
        maxLength = None
        path = None
        overlayId = None
        name = None


        def __init__(self, ramAddress, maxLength, path, overlayId=None,
                name=None):
            self.ramAddress = ramAddress
            self.maxLength = maxLength
            self.path = path
            self.overlayId = overlayId
            self.name = name

            self.compiler = code_cpp

            jsonPath = os.path.join(self.path, 'config.json')
            if os.path.isfile(jsonPath):
                with open(jsonPath, 'r', encoding='utf-8') as f:
                    self._json = json.load(f)
                self.ramAddress = int(self._json['ram_address'], 0)
                self.maxLength = int(self._json['max_length'], 0)
            else:
                self._json = {}


        def __repr__(self):
            maxLen = None
            if self.maxLength is not None:
                maxLen = hex(self.maxLength)
            return (f'{type(self).__name__}'
                f'({fmtAddr(self.ramAddress)}, {maxLen}, {repr(self.path)})')


        @property
        def codeAddress(self):
            """
            The address that the code should be based at. This is often
            the same as self.ramAddress, but not always.
            """
            # We have to add 4 bytes for every exported function (to
            # form the jump table).
            return self.ramAddress + 4 * len(self.exportsList)


        _sourcePaths = None
        @property
        def sourcePaths(self):
            """
            Return a list of paths to source files.
            """
            if self._sourcePaths is None:
                L = []
                for f in os.listdir(os.path.join(self.path, 'source')):
                    f2 = os.path.join(self.path, 'source', f)
                    if os.path.isfile(f2):
                        L.append(f2)
                self._sourcePaths = L
            return self._sourcePaths


        def pathsEndingWith(self, extensions):
            """
            Return a list of paths to only the source files that end
            with one of the extensions in the list provided.
            (Example: ['.txt', '.png'])
            """
            def isGood(fn):
                return any(fn.lower().endswith(g) for g in extensions)
            return [s for s in self.sourcePaths if isGood(s)]


        def reportSpaceUsed(self, amount):
            """
            Save a text file reporting the amount of space that was used
            to compile this part of the build.
            """

            if self.maxLength is None:
                maxSpace = '(unlimited)'
                leftSpace = '(unlimited)'
                pct = '(N/A)'
            else:
                maxSpace = fmtAddr(self.maxLength)
                leftSpace = fmtAddr(self.maxLength - amount)
                pct = f'%0.3f%%' % (amount / self.maxLength * 100)

            lines = []
            ap = lines.append
            ap(f'Compiled Code Space Report ({datetime.now()})')
            ap('-' * len(lines[-1]))
            ap('')
            ap(f'Total available space:                     {maxSpace}')
            ap(f'Space used during most recent compilation: {fmtAddr(amount)}')
            ap(f'Remaining available space:                 {leftSpace}')
            ap(f'Percentage of available space used:        {pct}')

            with open(os.path.join(self.path, 'space-report.txt'),
                    'w', encoding='utf-8') as f:
                f.write('\n'.join(lines))


        _exportsList = None
        @property
        def exportsList(self):
            """
            A list of all functions to be exported.
            """
            if self._exportsList is None:
                fp = os.path.join(self.path, 'exports.txt')
                if os.path.isfile(fp):
                    with open(fp, 'r', encoding='utf-8') as f:
                        self._exportsList = f.read().split('\n')
                        self._exportsList = [x for x in self._exportsList if x]
                else:
                    self._exportsList = []
            return list(self._exportsList)


        @property
        def romFileID(self):
            """
            The file ID where the code should be stored, according to
            config.json.
            """
            return self._json.get('file_id')


        def compile(self, project, metaFiles):
            """
            Compile this code unit with the proper compiler. Return the
            contents of newcode.bin and newcode.sym as a bytes and a
            string, respectively.
            """
            return self.compiler.compileUnit(project, self, metaFiles)


        def iterHeaderFiles(self):
            """
            Iterate over all header files this code unit uses.
            """
            return self.compiler.iterHeaderFiles(self)


    arenaOffs = 0x00000000

    def __init__(self, path, useCache=True):
        self.path = path
        self.useCache = useCache

        with open(self.file('arenaoffs.txt'), 'r', encoding='utf-8') as f:
            self.arenaOffs = int(f.read(), 16)

        codeFiles = self.getOriginalCode()

        # Load the various code units
        self.insertions = []
        self.newOverlays = []

        for thing in os.listdir(path):
            full = os.path.join(path, thing)
            if not os.path.isdir(full): continue

            if thing.startswith('insert_'):
                parts = thing.split('_')
                ramAddr = int(parts[1], 16)
                maxLen = int(parts[2], 16) - ramAddr
                inv = self.CodeUnit(ramAddr, maxLen, full)
                self.insertions.append(inv)

            elif thing.startswith('ov_'):
                parts = thing.split('_')
                ovId = int(parts[1], 16)
                name = '_'.join(parts[2:])

                # RAM address and max length will be automatically
                # set by CodeUnit's initializer
                inv = self.CodeUnit(None, None, full, ovId, name)
                self.newOverlays.append(inv)

        # Sort the code unit lists. This will determine the order in
        # which the units are compiled. Units can only reference code
        # from themselves and from other units compiled before them. So
        # this is a pretty important step.
        self.insertions.sort(key=lambda inv: inv.ramAddress)
        self.newOverlays.sort(key=lambda inv: (inv.ramAddress, inv.name))

        self.appended = None
        if os.path.isdir(self.file('append')):
            # Be sure to fill in .ramAddress before compiling this one!
            self.appended = self.CodeUnit(None, None,
                                          self.file('append'))

        # Prepare self.replaces
        self.replaces = []
        replacesFP = self.file('replaces.x')
        if os.path.isfile(replacesFP):

            with open(replacesFP, 'r', encoding='utf-8') as f:
                lines = f.read().split('\n')

            for line in lines:
                line = line.split('@')[0].strip()
                if not line: continue

                self.replaces.append(Replace.fromReplacesX(line))


    def file(self, *nameParts):
        """
        Return the full path to the file described. Pass the relative
        path to the file as separate arguments. Example usage:
        file('original', 'arm9.bin')
        """
        return os.path.join(self.path, *nameParts)


    _origFiles = None
    def getOriginalCode(self):
        """
        Construct a CodeFiles object from the "original" folder.
        This is a function rather than a property to make it clear that
        you'll get a new CodeFiles object each time you run this. That's
        important because CodeFiles objects are mutable.
        """
        if self._origFiles is None:
            self._origFiles = {}
            with open(self.file('original', 'arm9.bin'), 'rb') as f:
                self._origFiles['arm9data'] = f.read()
            with open(self.file('original', 'arm9ovt.bin'), 'rb') as f:
                self._origFiles['arm9ovtdata'] = f.read()
                arm9ovtdata = self._origFiles['arm9ovtdata']
            with open(self.file('original', 'arm7.bin'), 'rb') as f:
                self._origFiles['arm7data'] = f.read()
            with open(self.file('original', 'arm7ovt.bin'), 'rb') as f:
                self._origFiles['arm7ovtdata'] = f.read()
                arm7ovtdata = self._origFiles['arm7ovtdata']
            with open(self.file('original', 'header.bin'), 'rb') as f:
                self._origFiles['headerdata'] = f.read()
            with open(self.file('original', 'fnt.bin'), 'rb') as f:
                self._origFiles['fntdata'] = f.read()

            overlays = {}
            numOverlays = len(arm9ovtdata) // 32
            for i in range(numOverlays):
                ovId, = struct.unpack_from('<I', arm9ovtdata, i * 32)
                with open(self.file('original',
                                     'overlay9',
                                    f'overlay9_{ovId}.bin'), 'rb') as f:
                    overlays[ovId] = f.read()
            self._origFiles['overlays9'] = overlays

            overlays = {}
            numOverlays = len(arm7ovtdata) // 32
            for i in range(numOverlays):
                ovId, = struct.unpack_from('<I', arm7ovtdata, i * 32)
                with open(self.file('original',
                                     'overlay7',
                                    f'overlay7_{ovId}.bin'), 'rb') as f:
                    overlays[ovId] = f.read()
            self._origFiles['overlays7'] = overlays

        return CodeFiles(self._origFiles['arm9data'],
                         dict(self._origFiles['overlays9']),
                         self._origFiles['arm9ovtdata'],
                         self._origFiles['arm7data'],
                         dict(self._origFiles['overlays7']),
                         self._origFiles['arm7ovtdata'],
                         self._origFiles['headerdata'],
                         self._origFiles['fntdata'])


    _origSymbols = None
    def getOriginalMeta(self):
        """
        Construct a MetaFiles object.
        This is a function rather than a property to make it clear that
        you'll get a new MetaFiles object each time you run this. That's
        important because MetaFiles objects are mutable.
        """
        if self._origSymbols is None:
            self._origSymbols = []
            if os.path.isfile(self.file('game.map')):
                with open(self.file('game.map'), 'r', encoding='utf-8') as f:
                    d = f.read()
                for line in d.split('\n'):
                    line = line.strip()
                    if not line: continue
                    self._origSymbols.append(Symbol.fromGameMap(line))

        return MetaFiles(list(self._origSymbols), {})



class Symbol:
    """
    A name assigned to an address, which might be in an overlay.
    """
    name = ''
    address = 0x00000000
    overlayId = None
    mangled = False


    def __init__(self, name, address, overlayId=None):

        # Demangle mangled names
        if name.startswith('_Z'):
            # Mangled names look like these:
            # _Z12copyVRAMTilePvS_iiiiii
            # _ZL6keysDw
            # _Z19hook_020D0338_ov_08v
            # _Z9debugMenuv
            # They always start with _Z, which is rarely followed by
            # more letters. Then there's a number signifying the length
            # of the function name, which is followed by the name
            # itself. Then there's some type stuff that we don't care
            # about.

            def digit(c):
                try:
                    int(c)
                    return True
                except ValueError:
                    return False

            digits = ''
            idx = 2
            while not digit(name[idx]): idx += 1
            while digit(name[idx]):
                digits += name[idx]
                idx += 1

            nameLen = int(digits)
            nameStart = idx
            name = name[nameStart : nameStart+nameLen]
            mangled = True
        else:
            mangled = False

        self.name = name
        self.address = address
        self.overlayId = overlayId
        self.mangled = mangled


    def __str__(self):
        return f'{self.name} = {fmtAddr(self.address)};'


    def __repr__(self):
        overlayId = None
        if self.overlayId is not None:
            overlayId = hex(self.overlayId)
        return (f'{type(self).__name__}'
            f'({repr(self.name)}, {fmtAddr(self.address)}, {overlayId})')


    @classmethod
    def fromGameMap(cls, line):
        """
        Create a Symbol from a string in the format used by game.map.
        """
        line = line.replace('\t', ' ')
        name, addr = [part for part in line.split(' ') if part]
        addr = int(addr, 16)
        return cls(name, addr)


    @classmethod
    def parseNewcodeSym(cls, newcodeSym):
        """
        Parse a string representing newcode.sym, and return a list of
        Symbols.
        """
        syms = []

        # newcode.sym is pretty messy and hard to parse; this should do
        # a pretty good job, though.

        foundSymbolTable = False
        for line in newcodeSym.split('\n'):

            # Only parse lines following "SYMBOL TABLE:"
            if line == 'SYMBOL TABLE:':
                foundSymbolTable = True
                continue
            elif not foundSymbolTable:
                continue

            if '.text' not in line and '.data' not in line: continue

            addr = int(line[:8], 16)
            if addr == 0: continue

            line = line.replace('\t', ' ')
            spl = [x for x in line.split(' ') if x]

            name = spl[-1]
            if '*ABS*' in name: # No idea what this is
                continue
            if name.startswith('.'): # Sometimes it prepends a . for
                                     # some reason
                name = name[1:]
            else:
                if '.' in name: # Things like "filename.o"
                    continue
            if name.startswith('__'): # Internal compiler functions
                continue

            ovId = None
            if '_ov_' in name:
                idx = name.index('_ov_') + 4
                ovId = int(name[idx:idx+2], 16)

            syms.append(cls(name, addr, ovId))

        return syms



class Replace:
    """
    A request to place a value at a given address, which may be in an
    overlay.
    """
    address = 0x00000000
    overlayId = None
    value = None


    def __init__(self, address, overlayId, value):
        self.address = address
        self.overlayId = overlayId
        self.value = value


    def __str__(self):
        if isinstance(self.value, int):
            value = hex(self.value)
        else:
            value = self.value

        line = fmtAddr(self.address, False)
        if self.overlayId is not None:
            line += '_ov_%02X' % self.overlayId
        line += ': ' + value
        return line


    @classmethod
    def fromReplacesX(cls, line):
        """
        Create a Replace from a string in the format used by replaces.x.
        """
        spl = [part.strip() for part in line.split(':')]
        try:
            value = int(spl[1], 16)
        except:
            value = spl[1]
        if '_ov_' in spl[0]:
            idx = spl[0].index('_ov_') + 4
            overlayId = int(spl[0][idx:idx+2], 16)
        else:
            overlayId = None
        address = int(spl[0][:8], 16)
        return cls(address, overlayId, value)



class CodeFiles:
    """
    An object that keeps track of the current state of the compiled code
    files (arm9.bin, overlay files, etc).
    """

    def __init__(self,
                 arm9Data, arm9Overlays, arm9OverlayTable,
                 arm7Data, arm7Overlays, arm7OverlayTable,
                 headerData, fntData):
        arm9RamAddress, = struct.unpack_from('<I', headerData, 0x28)
        arm7RamAddress, = struct.unpack_from('<I', headerData, 0x38)

        self.arm9 = ndspy.code.MainCodeFile(arm9Data, arm9RamAddress)
        self.arm7 = ndspy.code.MainCodeFile(arm7Data, arm7RamAddress)
        self.header = headerData
        self.fnt = ndspy.fnt.load(fntData)

        def callback(ovID, fileID):
            return arm9Overlays[ovID]
        self.arm9Overlays = ndspy.code.loadOverlayTable(arm9OverlayTable, callback)

        def callback(ovID, fileID):
            return arm7Overlays[ovID]
        self.arm7Overlays = ndspy.code.loadOverlayTable(arm7OverlayTable, callback)


    def writeValue(self, address, value, processor=ndspy.Processor.ARM9, overlayId=None):
        """
        Write a value to a memory address in the specified overlay.
        (Use `None` for overlayId to refer to the main code file
        itself.)
        """
        mainCode = self.arm9 if processor == ndspy.Processor.ARM9 else self.arm7
        overlays = self.arm9Overlays if processor == ndspy.Processor.ARM9 else self.arm7Overlays

        if overlayId is None:
            for s in mainCode.sections:
                if self.contains(s, address):
                    struct.pack_into('<I', s.data,
                        address - s.ramAddress, value)
                    return

            for overlayId, ov in overlays.items():
                if self.contains(ov, address):
                    struct.pack_into('<I', ov.data,
                        address - ov.ramAddress, value)
                    return

        else:
            if overlayId not in overlays:
                raise RuntimeError(
                    f'Overlay {hex(overlayId)} does not exist :(')

            ov = overlays[overlayId]

            if not self.contains(ov, address):
                raise RuntimeError(f'Overlay {hex(overlayId)}'
                    f" doesn't contain {fmtAddr(address)} :(")
            
            struct.pack_into('<I', ov.data, address - ov.ramAddress, value)
            return

        raise RuntimeError(f'{fmtAddr(address)} could not be found :/')


    def readValue(self, address, processor=ndspy.Processor.ARM9, overlayId=None):
        """
        Read a value from a memory address in the specified overlay.
        (Use `None` for overlayId to refer to the main code file
        itself.)
        """
        mainCode = self.arm9 if processor == ndspy.Processor.ARM9 else self.arm7
        overlays = self.arm9Overlays if processor == ndspy.Processor.ARM9 else self.arm7Overlays

        if overlayId is None:
            for s in mainCode.sections:
                if self.contains(s, address):
                    return struct.unpack_from('<I', s.data,
                        address - s.ramAddress)[0]

            for overlayId, ov in overlays.items():
                if self.contains(ov, address):
                    return struct.unpack_from('<I', ov.data,
                        address - ov.ramAddress)[0]

        else:
            if overlayId not in overlays:
                raise RuntimeError(
                    f'Overlay {hex(overlayId)} does not exist :(')

            ov = overlays[overlayId]

            if not self.contains(ov, address):
                raise RuntimeError(f'Overlay {hex(overlayId)}'
                    f" doesn't contain {fmtAddr(address)} :(")
            
            return struct.unpack_from('<I', ov.data,
                                      address - ov.ramAddress)[0]

        raise RuntimeError(f'{fmtAddr(address)} could not be found :/')


    @staticmethod
    def contains(thing, address):
        """
        Check if thing contains address.
        """
        if hasattr(thing, 'ramSize'):
            ramSize = thing.ramSize
        else:
            ramSize = len(thing.data)
        return thing.ramAddress <= address < thing.ramAddress + ramSize


    def makeBuildtime(self, time=None, username=None):
        """
        Make the BUILDTIME file using the provided time (should be a
        datetime object) and username. Use the current system time and
        username as defaults.
        """
        gameID = self.header[0x0C:0x10].decode('ascii')

        if time is None:
            time = datetime.now()
        dt = time.strftime('%Y-%m-%d %H:%M:%S')

        if username is None:
            username = getpass.getuser()

        return f'{gameID}{dt}{username}'.encode('ascii')[:32]


    def makeArm9Ovt(self):
        """
        Put together and return an appropriate ARM9 overlay table file.
        """
        return ndspy.code.saveOverlayTable(self.arm9Overlays)

    def makeArm7Ovt(self):
        """
        Put together and return an appropriate ARM7 overlay table file.
        """
        return ndspy.code.saveOverlayTable(self.arm7Overlays)



class MetaFiles:
    """
    Keeps track of the most current metadata about the code files:
    symbols, and paths to header files.
    """

    symbols = None
    headers = None

    def __init__(self, symbols, headers):
        self.symbols = symbols
        self.headers = headers


    def makeSymbolMap(self):
        """
        Create a symbols.map file, like the game.map file used in
        projects.
        """

        # The column that addresses will be aligned to
        ADDRESS_COL = 64

        lines = []
        for sym in sorted(self.symbols, key=lambda s: s.address):
            line = sym.name

            # Add spaces to align the address, but ensure that there is
            # always at least one space added
            if len(line) < ADDRESS_COL:
                line += ' ' * (ADDRESS_COL - len(line))
            else:
                line += ' '

            line += fmtAddr(sym.address, False)

            lines.append(line)

        return '\n'.join(lines)


def saveToFolder(files, path, username=None):
    """
    Save a CodeFiles object to a folder.
    """

    def save(pathParts, data):
        with open(os.path.join(path, *pathParts), 'wb') as f:
            f.write(data)

    if not os.path.isdir(os.path.join(path, 'overlay9')):
        os.makedirs(os.path.join(path, 'overlay9'))
    if not os.path.isdir(os.path.join(path, 'other')):
        os.makedirs(os.path.join(path, 'other'))

    save(['arm9.bin'], files.arm9.save())
    save(['arm9ovt.bin'], files.makeArm9Ovt())

    for id, ov in files.arm9Overlays.items():
        save(['overlay9', f'overlay9_{id}.bin'], ov.save())

    save(['BUILDTIME'], files.makeBuildtime(username=username))


def saveAsNmcp(files, path, username=None):
    """
    Save a CodeFiles object as a .nmcp file.
    """

    # I'm ensuring that the files are sorted by ID because I feel like
    # it. It's not a requirement or anything. :P

    nmcpFilesDict = {}

    # -1: fnt.bin
    # -2: fat.bin
    nmcpFilesDict[-3] = files.makeArm9Ovt() # arm9ovt.bin
    # -4: arm7ovt.bin
    nmcpFilesDict[-5] = files.arm9.save() # arm9.bin
    # -6: arm7.bin
    # -7: rsasig.bin
    # -8: header.bin

    for id, ov in files.arm9Overlays.items():
        nmcpFilesDict[ov.fileID] = ov.save()

    nmcpFilesDict[files.fnt['BUILDTIME']] = files.makeBuildtime(username=username)

    nmcpFiles = sorted(list(nmcpFilesDict.items()), key=lambda item: item[0])

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    HEADER_STRING = b'NSMBe4 Code Patch'

    nmcp = bytearray([len(HEADER_STRING)])
    nmcp.extend(HEADER_STRING)

    for id, data in nmcpFiles:
        nmcp.extend(struct.pack('<BhI', 1, id, len(data)))
        nmcp.extend(data)

    nmcp.extend(b'\0')

    with open(path, 'wb') as f:
        f.write(nmcp)


def insertIntoRom(files, path, compress=False, username=None):
    """
    Insert a CodeFiles object into a rom.
    """

    with open(path, 'rb') as f:
        romData = f.read()

    rom = ndspy.rom.NintendoDSRom(romData)

    if compress:
        print('Compressing ARM9')
    rom.arm9 = files.arm9.save(compress)

    # ARM7 is never compressed.
    rom.arm7 = files.arm7.save()

    for id, ov in files.arm9Overlays.items():
        if compress:
            print(f'Compressing overlay {id} / {max(files.arm9Overlays)}')
        rom.files[ov.fileID] = ov.save(compress)

    rom.arm9OverlayTable = files.makeArm9Ovt()
    rom.arm7OverlayTable = files.makeArm7Ovt()

    # Try to overwrite an existing BUILDTIME file if possible.
    # Sadly, we can't insert a new BUILDTIME without potentially messing
    # up existing file IDs.
    buildtimeData = files.makeBuildtime(username=username)
    if 'BUILDTIME' in rom.filenames:
        rom.files[rom.filenames['BUILDTIME']] = buildtimeData

    newRomData = rom.save()

    with open(path, 'wb') as f:
        f.write(newRomData)


def applyChanges(project, codeUnit, files, meta, append=False):
    """
    Run Make for the code unit given, and update files and meta with
    the output.
    """

    extraSection = files.extraArm9DataSection

    if append:
        codeUnit.ramAddress = (extraSection.ramAddress
                                 + len(extraSection.data))

    codebin, codesym = codeUnit.compile(project, meta)

    if append:
        extraSection.data.extend(codebin)

    # Now we need to deal with symbols with names that indicate that
    # they should hook into existing game code.

    syms = Symbol.parseNewcodeSym(codesym)

    alreadyProcessedHooks = set(s.name for s in meta.symbols)

    for sym in syms:
        # If the symbol is a nsub or repl or hook, apply that
        if any(sub in sym.name for sub in ['nsub_', 'repl_', 'hook_']):
            if sym.name in alreadyProcessedHooks: continue

            cmd = 'nsub'
            if 'repl' in sym.name:
                cmd = 'repl'
            elif 'hook' in sym.name:
                cmd = 'hook'

            start = sym.name.index(cmd) + 5
            ramAddr = int(sym.name[start : start+8], 16)

            ovId = None
            if '_ov_' in sym.name:
                idx = sym.name.index('_ov_') + 4
                ovId = int(sym.name[idx : idx+2], 16)

            if cmd == 'nsub':
                val = makeBranchOpcode(ramAddr, sym.address, False)
            elif cmd == 'repl':
                val = makeBranchOpcode(ramAddr, sym.address, True)
            elif cmd == 'hook':
                extraData = extraSection.data
                hookAddr = extraSection.ramAddress + len(extraData)

                val = makeBranchOpcode(ramAddr, hookAddr, False)

                originalOpcode = files.readValue(ramAddr, ndspy.Processor.ARM9, ovId)
                newOpcode = rebaseInstructionAtAddress(originalOpcode,
                                                       ramAddr,
                                                       hookAddr)

                extraData.extend(struct.pack('<I', newOpcode))
                hookAddr += 4
                # push {r0-r12, r14}
                extraData.extend(struct.pack('<I', 0xE92D5FFF))
                hookAddr += 4
                extraData.extend(struct.pack('<I',
                    makeBranchOpcode(hookAddr, sym.address, True)))
                hookAddr += 4
                # pop {r0-r12, r14}
                extraData.extend(struct.pack('<I', 0xE8BD5FFF))
                hookAddr += 4
                extraData.extend(struct.pack('<I',
                    makeBranchOpcode(hookAddr, ramAddr + 4, False)))
                hookAddr += 4

            files.writeValue(ramAddr, val, ndspy.Processor.ARM9, ovId)

    # Update meta

    for fname, fdata in codeUnit.iterHeaderFiles():
        meta.headers[fname] = fdata

    existingSymbols = set(s.name for s in meta.symbols)
    symsNotAdded = []
    for s in syms:
        if s.name in existingSymbols:
            symsNotAdded.append(s)
        else:
            meta.symbols.append(s)

    return codebin, symsNotAdded


def compileProject(project):
    """
    Compile a project, and return the completed CodeFiles and MetaFiles
    objects.
    """

    files = project.getOriginalCode()
    meta = project.getOriginalMeta()

    # Add the extra data section to arm9.bin
    arenaOffsValue = files.readValue(project.arenaOffs, ndspy.Processor.ARM9)
    files.arm9.sections.append(ndspy.code.MainCodeFile.Section(b'', arenaOffsValue, 0))
    files.extraArm9DataSection = files.arm9.sections[-1]
    extraSection = files.extraArm9DataSection

    # Add overlay data to meta (headers and the symbols table)
    a, b = code_cpp.combineExports(project.newOverlays)
    meta.headers[a] = b
    for inv in project.newOverlays:
        for i, export in enumerate(inv.exportsList):
            sym = Symbol(export, inv.ramAddress + 4 * i, inv.overlayId)
            meta.symbols.append(sym)

    # Compile the code insertions
    for inv in project.insertions:
        newcode, _ = applyChanges(project, inv, files, meta)

        # Insert the new code
        destAddress = inv.codeAddress
        destLen = len(newcode)
        for s in files.arm9.sections:
            sAddr = s.ramAddress
            sLen = len(s.data)
            if sAddr <= destAddress and sAddr + sLen > destAddress + destLen:
                s.data[destAddress-sAddr : destAddress-sAddr+destLen] = newcode
                break

    # Compile the appended code
    if project.appended is not None:
        applyChanges(project, project.appended, files, meta, True)

    # Compile new overlays
    for inv in project.newOverlays:
        newcode, symsNotAdded = applyChanges(project, inv, files, meta)

        # Put the overlay data together

        ovData = bytearray()

        for i, export in enumerate(inv.exportsList):
            srcAddr = inv.ramAddress + len(ovData)
            compiledSym = [s for s in symsNotAdded if s.name == export][0]
            destAddr = compiledSym.address
            ovData.extend(struct.pack('<I',
                makeBranchOpcode(srcAddr, destAddr, False)))

        assert inv.ramAddress + len(ovData) == inv.codeAddress
        ovData.extend(newcode)

        newOverlay = ndspy.code.Overlay(ovData,
                                        inv.ramAddress,
                                        len(ovData),
                                        0,
                                        0,
                                        0,
                                        inv.romFileID,
                                        len(ovData),
                                        2) # TEMP
        files.arm9Overlays[inv.overlayId] = newOverlay


    # ========================
    # Handle replaces

    repls = list(project.replaces)

    # Symbol replaces
    for sym in meta.symbols:
        for r in list(repls):
            if r.value == sym.name:
                files.writeValue(r.address, sym.address, ndspy.Processor.ARM9, r.overlayId)
                repls.remove(r)

    # Constant replaces
    for r in list(repls):
        if isinstance(r.value, int):
            files.writeValue(r.address, r.value, ndspy.Processor.ARM9, r.overlayId)
            repls.remove(r)

    # Throw an error if we missed any replaces
    if repls:
        print('Some replaces were missed:')
        for r in repls:
            print(r)
        raise RuntimeError('Some replaces were missed.')


    # Update arenaOffs to account for the extra arm9 section
    oldArenaOffsValue = files.readValue(project.arenaOffs, ndspy.Processor.ARM9)
    newArenaOffsValue = (oldArenaOffsValue
        + len(files.extraArm9DataSection.data))
    files.writeValue(project.arenaOffs, newArenaOffsValue, ndspy.Processor.ARM9)

    return files, meta


def main():
    """
    Main function to be run.
    """

    parser = argparse.ArgumentParser(
        description='Magigoomba: compile code patches for Nintendo DS games.')

    # Input and symbols
    parser.add_argument('project_dir',
                        help='the directory of the project')
    parser.add_argument('-s', '--symbols',
                        metavar='SYMBOLSFILE',
                        help='the symbols.map file to write symbols to')

    # Build options
    parser.add_argument('-u', '--username',
                        help='the username to put in the BUILDTIME file'
                             " (default is your user account's username)")
    parser.add_argument('-c', '--clearcache',
                        action='store_true',
                        help='clear the cache before compiling')
    parser.add_argument('-r', '--release',
                        action='store_true',
                        help='compress the generated code files')

    # Output options
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-f', '--outputfolder',
                       help='the output folder to save files to')
    group.add_argument('-p', '--nmcp',
                       metavar='NMCPFILE',
                       help='the NMCP patch file to export as')
    group.add_argument('-n', '--nds',
                       metavar='NDSFILE',
                       help='the .nds ROM file to insert into')

    args = parser.parse_args()

    # Now actually compile the things!

    files, meta = compileProject(Project(os.path.expanduser(args.project_dir),
                                         not args.clearcache))

    if args.outputfolder is not None:
        saveToFolder(files, os.path.expanduser(args.outputfolder), args.username)
    elif args.nmcp is not None:
        saveAsNmcp(files, os.path.expanduser(args.nmcp), args.username)
    elif args.nds is not None:
        insertIntoRom(files, os.path.expanduser(args.nds), args.release, args.username)

    if args.symbols is not None:
        with open(os.path.expanduser(args.symbols), 'w', encoding='utf-8') as f:
            f.write(meta.makeSymbolMap())


if __name__ == '__main__':
    main()