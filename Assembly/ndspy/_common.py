
import struct

import crcmod


# Nintendo DS standard file header:
NDS_STD_FILE_HEADER = struct.Struct('<4sHHIHH')
# - Magic
# - BOM
# - Version (generally 1)
# - File size (including this header)
# - Size of this header (16)
# - Number of blocks


def crc16(data):
    return crcmod.predefined.mkCrcFun('modbus')(data)


def loadNullTerminatedStringFrom(
        data, offset, charWidth=1, encoding='latin-1'):
    """
    Load a null-terminated string from data at offset, with the options
    given
    """
    end = data.find(b'\0' * charWidth, offset)
    return data[offset:end].decode(encoding)


def listFind(L, *args):
    try:
        return L.index(*args)
    except ValueError:
        return -1


def noteName(value):
    note = value - 60
    letter = 'ccddeffggaab'[note % 12]
    sharp  = ' ♯ ♯  ♯ ♯ ♯ '[note % 12].strip()
    if note < -12:
        letter = letter.upper()
    accents = ''
    while note >= 0:
        accents += "'"
        note -= 12
    while note < -24:
        accents += '͵'
        note += 12
    return letter + sharp + accents
