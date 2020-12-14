
import enum
import struct

from . import _common

class SWAV:
    """
    A streamed music format. Normally used only for short sounds.
    """

    class WaveType(enum.IntEnum):
        PCM8 = 0
        PCM16 = 1
        ADPCM = 2

    def __init__(self, file=None):
        self.waveType = self.WaveType.PCM8
        self.loops = False
        self.sampleRate = 8000
        self.time = 0
        self.loopOffset = 0
        self.totalLength = 0
        self.data = b''

        if file is not None:
            if not file.startswith(b'SWAV'):
                raise ValueError("Wrong magic (should be b'SWAV', instead "
                                 f'found {file[:4]})')

            self._init_from_data(file)

    def _init_from_data(self, file):
        """
        Initialize the SWAV from file data.
        """

        magic, bom, version, filesize, headersize, numblocks = \
            _common.NDS_STD_FILE_HEADER.unpack_from(file, 0)
        if version != 0x100:
            raise ValueError(f'Unsupported SWAV version: {version}')
        assert magic == b'SWAV'

        dataMagic, dataSize = struct.unpack_from('<4sI', file, 0x10)
        assert dataMagic == b'DATA'

        (waveType, self.loops, self.sampleRate, self.time, self.loopOffset,
            self.totalLength) = struct.unpack_from('<B?3HI', file, 0x18)
        self.waveType = self.WaveType(waveType)

        self.data = file[0x24:]

    def save(self):
        data = bytearray()
        data.extend(_common.NDS_STD_FILE_HEADER.pack(
            b'SWAV', 0xFEFF, 0x100, 0x24 + len(self.data), 0x10, 1))
        data.extend(struct.pack(
            '<4sI', b'DATA', 0x14 + len(self.data)))
        data.extend(struct.pack(
            '<B?3HI', self.waveType, self.loops, self.sampleRate, self.time,
            self.loopOffset, self.totalLength))
        data.extend(self.data)
        return data


    def __str__(self):
        try:
            typeName = self.WaveType(self.waveType).name
        except Exception:
            typeName = f'type {hex(self.waveType)}'

        looped = ' looped' if self.loops else ''

        return f'<swav {typeName} {self.sampleRate}Hz duration={self.time}{looped}>'

    def __repr__(self):
        if len(self.data) > 0x80:
            data = repr(bytes(self.data[:0x80])) + '...'
        else:
            data = repr(bytes(self.data))
        return f'{self.__class__.__name__}({data})'