
import struct

from . import _common
from . import soundWave

class SWAR:
    """
    A wave archive. A list of SWAVs, really.
    """
    # When saving SDAT, two otherwise identical SWARs will share data
    # only if their dataMergeOptimizationIDs are the same.
    # You can pretty safely ignore this.
    dataMergeOptimizationID = 0

    def __init__(self, file=None, unk02=0):
        self.unk02 = unk02
        self.waves = []

        if file is not None:
            if not file.startswith(b'SWAR'):
                raise ValueError("Wrong magic (should be b'SWAR', instead "
                                 f'found {file[:4]})')

            self._init_from_data(file)


    def _init_from_data(self, data):
        """
        Initialize the SWAR from file data.
        """
        magic, bom, version, filesize, headersize, numblocks = \
            _common.NDS_STD_FILE_HEADER.unpack_from(data, 0)
        if version != 0x100:
            raise ValueError(f'Unsupported SWAR version: {version}')
        assert magic == b'SWAR'

        dataMagic, dataSize, swavCount = \
            struct.unpack_from('<4sI32xI', data, 0x10)
        assert dataMagic == b'DATA'

        dataArrayPos = 0x3C
        for i in range(swavCount):
            swavOffset, nextOffset = \
                struct.unpack_from('<II', data, dataArrayPos)
            dataArrayPos += 4
            if i == swavCount - 1:
                # DON'T use len(data) here -- there can sometimes be
                # extra pad at the end, which will mess this up!
                nextOffset = filesize

            swavData = data[swavOffset:nextOffset]

            waveType, loopFlag, sampleRate, time, loopOffset, totalLength = \
                struct.unpack_from('<B?3HI', swavData)

            swav = soundWave.SWAV()
            swav.waveType = swav.WaveType(waveType)
            swav.loops = loopFlag
            swav.sampleRate = sampleRate
            swav.time = time
            swav.loopOffset = loopOffset
            swav.totalLength = totalLength
            swav.data = swavData[0xC:]

            self.waves.append(swav)


    @classmethod
    def fromWaves(cls, waves, unk02=0):
        obj = cls(unk02=unk02)
        obj.waves = waves
        return obj


    def save(self):
        """
        Return all the parts of the SWAR, in the same format as the
        arguments to __init__.
        """
        fileLen = 0x3C + sum(4 + 0xC + len(s.data) for s in self.waves)

        data = bytearray()
        data.extend(_common.NDS_STD_FILE_HEADER.pack(
            b'SWAR', 0xFEFF, 0x100, fileLen, 0x10, 1))
        data.extend(struct.pack(
            '<4sI32xI', b'DATA', fileLen - 0x10, len(self.waves)))

        lenOffsetsTable = 4 * len(self.waves)
        offsets = []
        swavsTable = bytearray()

        for swav in self.waves:
            offsets.append(0x3C + lenOffsetsTable + len(swavsTable))
            swavData = swav.save()[0x18:] # Chop off the file header
                                          # but not the SWAVInfo struct
            swavsTable.extend(swavData)

        offsetsTable = struct.pack(f'<{len(offsets)}I', *offsets)
        data.extend(offsetsTable)
        data.extend(swavsTable)

        return (data,
                self.unk02)


    def __str__(self):
        linesList = [f'<swar']

        maxNumLen = len(str(len(self.waves) - 1))

        for i, wav in enumerate(self.waves):
            linesList.append(f'    [{i:{maxNumLen}}] {wav}')

        linesList.append('>')
        return '\n'.join(linesList)

    def __repr__(self):
        return f'{self.__class__.__name__}.fromWaves({self.waves})'