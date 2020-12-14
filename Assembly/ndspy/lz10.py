import struct

from . import _lzCommon


def decompress(data):
    """
    Decompress LZ10-compressed data.
    This code is ported from NSMBe, which was converted from Elitemap.
    """
    if data[0] != 0x10:
        raise TypeError("This isn't a LZ10-compressed file.")

    dataLen = struct.unpack_from('<I', data)[0] >> 8

    out = bytearray(dataLen)
    inPos, outPos = 4, 0

    while dataLen > 0:
        d = data[inPos]; inPos += 1

        if d:
            for i in range(8):
                if d & 0x80:
                    thing, = struct.unpack_from('>H', data, inPos); inPos += 2

                    length = (thing >> 12) + 3
                    offset = thing & 0xFFF
                    windowOffset = outPos - offset - 1

                    for j in range(length):
                        out[outPos] = out[windowOffset]
                        outPos += 1; windowOffset += 1; dataLen -= 1

                        if dataLen == 0:
                            return bytes(out)

                else:
                    out[outPos] = data[inPos]
                    outPos += 1; inPos += 1; dataLen -= 1

                    if dataLen == 0:
                        return bytes(out)

                d <<= 1
        else:
            for i in range(8):
                out[outPos] = data[inPos]
                outPos += 1; inPos += 1; dataLen -= 1

                if dataLen == 0:
                    return bytes(out)

    return bytes(out)


def compress(data):
    """
    LZ10-compress data.
    This code is ported from NSMBe.
    """
    compressed, _, _ = _lzCommon.compress(data, 1, 0x1000, 18, True, False)
    compressed[:0] = struct.pack('<I', (len(data) << 8) | 0x10)
    return compressed
