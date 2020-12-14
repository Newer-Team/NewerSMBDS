
import collections
import struct

from . import _common
from . import fnt


def load(data):
    """
    Load a NARC from data.
    Returns the filename table and the list of files.
    """
    # Read the standard header
    magic, bom, version, filesize, headersize, numblocks = \
        _common.NDS_STD_FILE_HEADER.unpack_from(data, 0)
    if version != 1:
        raise ValueError(f'Unsupported NARC version: {version}')

    if magic != b'NARC':
        raise ValueError("Wrong magic (should be b'NARC', instead found "
                         f'{magic})')

    # Read the file allocation block
    fatbMagic, fatbSize, fileCount = struct.unpack_from('<4sII', data, 0x10)
    assert fatbMagic == b'FATB'[::-1]

    # Read the file name block
    fntbOffset = 0x10 + fatbSize
    fntbMagic, fntbSize = struct.unpack_from('<4sI', data, fntbOffset)
    assert fntbMagic == b'FNTB'[::-1]

    # Get the data from the file data block before continuing
    fimgOffset = fntbOffset + fntbSize
    fimgMagic, gmifSize = struct.unpack_from('<4sI', data, fimgOffset)
    assert fimgMagic == b'FIMG'[::-1]
    rawDataOffset = fimgOffset + 8

    # Read the file datas
    fileList = []
    for i in range(fileCount):
        startOffset, endOffset = struct.unpack_from('<II', data, 0x1C + 8 * i)
        fileList.append(data[rawDataOffset+startOffset : rawDataOffset+endOffset])

    # Parse the filenames
    names = fnt.load(data[fntbOffset + 8 : fntbOffset + fntbSize])

    return names, fileList


def save(filenames, fileList):
    """
    Save a NARC filenames table and file list back to data.
    """

    # Prepare the filedata and file allocation table block
    fimgData = bytearray(8)

    fatbData = bytearray()
    fatbData.extend(struct.pack('<4sII',
        b'FATB'[::-1], 0x0C + 8 * len(fileList), len(fileList)))

    # Write data into the FIMG and FAT blocks
    for i, fd in enumerate(fileList):
        startOff = len(fimgData) - 8
        fimgData.extend(fd)
        endOff = startOff + len(fd)
        fatbData.extend(struct.pack('<II', startOff, endOff))
        while len(fimgData) % 4:
            fimgData.append(0)

    # Put the header on the FIMG block
    struct.pack_into('<4sI', fimgData, 0, b'FIMG'[::-1], len(fimgData))

    # Assemble the filename table block
    nameTable = bytearray(fnt.save(filenames))
    while len(nameTable) % 4:
        nameTable.append(0xFF)
    fntbData = struct.pack('<4sI', b'FNTB'[::-1], len(nameTable) + 8) + nameTable

    # Put everything together and return.
    data = bytearray(0x10)
    data.extend(fatbData)
    data.extend(fntbData)
    data.extend(fimgData)
    _common.NDS_STD_FILE_HEADER.pack_into(
        data, 0, b'NARC', 0xFEFF, 1, len(data), 0x10, 3)
    return bytes(data)