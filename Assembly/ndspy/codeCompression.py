
from . import _lzCommon

import struct


def _detectAppendedData(data):
    """
    Attempt to check if there's any appended data at the end of the
    given data. Returns an integer representing the amount of such data
    if so, or None if the data doesn't seem to be compressed.
    """
    for possibleAmt in range(0, 0x20, 4):
        try:
            headerLen = data[-5 - possibleAmt]
        except IndexError:
            return None

        compLenHeaderLen, extraSize = struct.unpack_from('<II', data, len(data) - possibleAmt - 8)
        headerLen = compLenHeaderLen >> 24
        compressedLen = compLenHeaderLen & 0xFFFFFF

        if headerLen < 8: continue
        if compressedLen > len(data): continue
        for byte in data[-possibleAmt - headerLen  : -possibleAmt - 8]:
            if byte != 0xFF:
                continue

        return possibleAmt


def decompress(data):
    """
    Decompress code data.
    This code is ported from DSDecmp:
    http://www.romhacking.net/utilities/789/
    https://github.com/Barubary/dsdecmp
    """

    appendedDataAmount = _detectAppendedData(data)

    if appendedDataAmount is None:
        # Probably not compressed.
        return data

    if appendedDataAmount == 0:
        appendedData = b''
    else:
        appendedData = data[-appendedDataAmount:]
        data = data[:-appendedDataAmount]

    # If extraSize (the last value in the header) is 0, the data is not
    # actually compressed
    if data[-4:] == b'\0\0\0\0':
        return data

    # Read the header
    compLenHeaderLen, extraSize = struct.unpack_from('<II', data, len(data) - 8)
    headerLen = compLenHeaderLen >> 24
    compressedLen = compLenHeaderLen & 0xFFFFFF

    if len(data) < headerLen:
        raise ValueError(f'File is too small for header ({len(data)} < {headerLen})')

    if compressedLen > len(data):
        raise ValueError("Compressed length doesn't fit in the input file")

    # The rest of the header should be all 0xFF's
    for byte in data[-headerLen:-8]:
        if byte != 0xFF:
            raise ValueError("Header padding isn't entirely 0xFF")


    # Format description:
    #
    # Code LZ compression is basically just LZ-0x10 compression.
    # However, the order of reading is reversed: the compression starts
    # at the end of the file. Assuming we start reading at the end
    # towards the beginning, the format is:
    #
    # u32 extraSize            | decompressed data size = file length
    #                          | (including header) + this value
    # u8 headerLen             |
    # u24 compressedLen        | Can be less than file size (without
    #                          | header): if so, the rest of the file is
    #                          | uncompressed. It may also be the file
    #                          | size.
    # u8[headerSize-8] padding | 0xFF's
    # 
    # 0x10-like-compressed data follows (without the usual 4-byte
    # header). The only difference is that 2 should be added to the DISP
    # value in compressed blocks to get the proper value. The u32 and
    # u24 are read most significant byte first. If extraSize is 0, there
    # is no headerSize, decompressedLength or padding: the data starts
    # immediately, and is uncompressed.
    # 
    # arm9.bin has 3 extra u32 values at the 'start' (ie: end of the
    # file), which may be ignored (and are ignored here). These 12 bytes
    # also should not be included in the computation of the output size.

    # The compressed size is sometimes the file size
    if compressedLen >= len(data):
        compressedLen = len(data)

    # The first part of the file, not included in compressedLen, is not
    # compressed, and should be ignored.
    passthroughLen = len(data) - compressedLen
    passthroughData = data[:passthroughLen]

    # Then there's the compressed data. Also make a bytearray where
    # we'll be putting the decompressed data.
    compData = data[passthroughLen : passthroughLen + compressedLen - headerLen]
    decompData = bytearray(len(data) + extraSize - passthroughLen)

    # Main decompression loop
    currentOutSize = 0
    decompLen = len(decompData)
    readBytes = 0
    flags = 0
    mask = 1
    while currentOutSize < decompLen:

        # Update the mask. If all flag bits have been read, get a new
        # set.
        if mask == 1:
            if readBytes >= compressedLen:
                raise RuntimeError('Not enough data to decompress')

            flags = compData[-1 - readBytes]
            readBytes += 1

            mask = 0x80
        else:
            mask >>= 1

        # Bit = 1 means it's compressed
        if flags & mask:
            # Get length and displacement ("disp") values from the next 2 bytes
            if readBytes + 1 >= len(data):
                raise RuntimeError('Not enough data to decompress')

            byte1 = compData[-1 - readBytes]; readBytes += 1
            byte2 = compData[-1 - readBytes]; readBytes += 1

            # The number of bytes to copy
            length = (byte1 >> 4) + 3

            # Where the bytes should be copied from (relatively)
            disp = (((byte1 & 0x0F) << 8) | byte2) + 3

            if disp > currentOutSize:
                if currentOutSize < 2:
                    raise RuntimeError(
                        'Cannot go back more than already written; '
                        f'attempted to go back {hex(disp)} bytes when only '
                        f'{hex(currentOutSize)} bytes have been written')

                # HACK. This seems to produce valid files, but isn't the
                # most elegant solution. Although this *could* be the
                # actual way to use a disp of 2 in this format, as,
                # otherwise, the minimum would be 3 (and 0 is undefined,
                # and 1 is less useful).
                disp = 2

            bufIdx = currentOutSize - disp
            for i in range(length):
                next = decompData[-1 - bufIdx]
                bufIdx += 1

                decompData[-1 - currentOutSize] = next
                currentOutSize += 1

        else:
            if readBytes > len(data):
                raise RuntimeError('Not enough data to decompress')

            next = compData[-1 - readBytes]
            readBytes += 1

            decompData[-1 - currentOutSize] = next
            currentOutSize += 1

    return passthroughData + decompData + appendedData


def compress(data, isArm9=False, lookAhead=False):
    """
    Compress code data.
    This code is ported (with substantial changes) from Nintendo DS/GBA
    Compressors by CUE:
    https://gbatemp.net/threads/nintendo-ds-gba-compressors.313278/
    http://www.romhacking.net/utilities/826/
    """
    if isArm9:
        prefix = data[:0x4000]
        data = data[0x4000:]
    else:
        prefix = b''

    return bytes(prefix + _compress(data))


def _compress(data):

    compressed, ignorableD, ignorableC = \
        _lzCommon.compress(bytes(reversed(data)), 3, 0x1002, 18, False, True)
    compressed = bytearray(reversed(compressed))

    if not compressed or (len(data) + 4 < ((len(compressed) + 3) & ~4) + 8):
        # Compressed size too large -- copy the raw data over instead

        compressed = bytearray(data)

        while len(compressed) % 4:
            compressed.append(0)

        for i in range(4):
            compressed.append(0)

        return compressed

    else:
        # Remove ignorable data (i.e. data that can remain uncompressed
        # without making the overall output file size larger), and tack
        # on the header at the end

        actualCompLen = len(compressed) - ignorableC
        headerLen = 8
        compressed = data[:ignorableD] + compressed[ignorableC:]
        extraLen = len(data) - len(compressed)

        while len(compressed) % 4:
            compressed.append(0xFF)
            headerLen += 1

        ptr = len(compressed)

        compressed.extend(b'\0' * 8)
        struct.pack_into('<I', compressed, ptr, actualCompLen + headerLen)
        compressed[ptr + 3] = headerLen
        ptr += 4
        struct.pack_into('<I', compressed, ptr, extraLen - headerLen)

    return compressed