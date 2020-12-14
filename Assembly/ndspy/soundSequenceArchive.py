
import struct

from . import _common
from . import soundSequence


class SSAR:
    """
    A sequence archive. This is a collection of sequences. In practice,
    this is mainly used for sound effects.
    """
    # When saving SDAT, two otherwise identical SSARs will share data
    # only if their dataMergeOptimizationIDs are the same.
    # You can pretty safely ignore this.
    dataMergeOptimizationID = 0

    _events = None
    _sequences = None

    def __init__(self, file=None, unk02=0, names=None):
        self.unk02 = unk02

        if file is not None:
            if not file.startswith(b'SSAR'):
                raise ValueError("Wrong magic (should be b'SSAR', instead"
                                 f' found {file[:4]})')

            sequenceCount, = struct.unpack_from('<I', file, 0x1C)

            self._file = file
            self._names = list(names) if names is not None else []
            while len(self._names) < sequenceCount:
                self._names.append(str(len(self._names)))
            self._parsed = False
        else:
            self._events = []
            self._sequences = []
            self._parsed = True

    @classmethod
    def fromEventsAndSequences(cls, events, sequences, unk02=0):
        obj = cls(unk02=unk02)
        obj.events = events
        obj.sequences = sequences
        return obj

    @property
    def events(self):
        if not self._parsed:
            raise ValueError('You must parse the SSAR with .parse() before you'
                             ' can access .events!')
        return self._events
    @events.setter
    def events(self, value):
        self._events = value

    @property
    def sequences(self):
        if not self._parsed:
            raise ValueError('You must parse the SSAR with .parse() before you'
                             ' can access .sequences!')
        return self._sequences
    @sequences.setter
    def sequences(self, value):
        self._sequences = value

    def parse(self):
        """
        Actually process the file data.
        """
        if self._parsed: return
        self._events = []
        self._sequences = []
        self._parsed = True
        self._init_from_data(self._file, self._names)
        del self._file
        del self._names

    def _init_from_data(self, data, symbols):
        """
        Initialize the SSAR from file data.
        """

        magic, bom, version, filesize, headersize, numblocks = \
            _common.NDS_STD_FILE_HEADER.unpack_from(data, 0)
        if version != 0x100:
            raise ValueError(f'Unsupported NARC version: {version}')
        assert magic == b'SSAR'

        dataMagic, dataSize, dataOffset, dataCount = \
            struct.unpack_from('<4s3I', data, 0x10)
        assert dataMagic == b'DATA'

        sseqData = data[dataOffset:]

        startOffs = []
        startOffs2SSEQ = {}
        dataArrayPos = 0x20
        for i in range(dataCount):
            (sequenceOffset, bankID, volume, channelPressure,
                    polyphonicPressure, play) = \
                struct.unpack_from('<iH4B', data, dataArrayPos)
            dataArrayPos += 0xC

            sseq = SSARSequence(None, bankID, volume,
                channelPressure, polyphonicPressure, play)
            if sequenceOffset != -1:
                startOffs.append(sequenceOffset)

                # Put lists in the dictionary, because there can be
                # multiple SSEQs referencing the same offset
                if sequenceOffset not in startOffs2SSEQ:
                    startOffs2SSEQ[sequenceOffset] = []
                startOffs2SSEQ[sequenceOffset].append(sseq)

            if i < len(symbols):
                symb = symbols[i]
            else:
                symb = None

            self.sequences.append((symb, sseq))

        self.events, startEvents = soundSequence.read_sequence_events(
            sseqData, startOffs)

        for event, originalOff in zip(startEvents, startOffs):
            for sseq in startOffs2SSEQ[originalOff]:
                sseq.firstEvent = event


    def save(self):
        """
        Return all the parts of the SSAR, in the same format as the
        arguments to __init__.
        """
        if not self._parsed:
            return (self._file, self.unk02, self._names)

        startEvents = []
        seq2StartEvent = {}
        for seqName, seq in self.sequences:
            if seq.firstEvent is not None:
                startEvents.append(seq.firstEvent)
                seq2StartEvent[seq] = seq.firstEvent
        sseqData, startOffs = soundSequence.save_sequence_events(self.events,
                                                                 startEvents)

        tableData = bytearray()

        for name, sseq in self.sequences:
            if sseq in seq2StartEvent:
                off = startOffs[startEvents.index(seq2StartEvent[sseq])]
            else:
                off = -1
            s = struct.pack('<iH4B2x', off, *(sseq.save()[1:]))
            tableData.extend(s)

        dataOffset = 0x20 + len(self.sequences) * 0xC
        fileLen = dataOffset + len(sseqData)

        data = bytearray()
        data.extend(_common.NDS_STD_FILE_HEADER.pack(b'SSAR', 0xFEFF, 0x100, fileLen, 16, 1))

        data.extend(struct.pack('<4s3I',
            b'DATA', fileLen - 0x10, dataOffset, len(self.sequences)))

        data.extend(tableData)
        data.extend(sseqData)

        return (data,
                self.unk02,
                [name for (name, sseq) in self.sequences])


    def sequenceNames(self):
        """
        Return a list of the names of the sequences.
        This is roughly equivalent to
        [name for (name, sseq) in ssar.sequences]
        except that it will always work, whether the SSAR has been
        parsed or not.
        """
        if not self._parsed:
            return list(self._names)
        return [name for (name, sseq) in self.sequences]


    def __str__(self):
        if not self._parsed:
            return f'<ssar unparsed [{", ".join(self._names)}]>'

        linesList = ['<ssar']
        linesList.append(soundSequence._printSequenceEventList(
            self.events,
            {name: sseq.firstEvent for (name, sseq) in self.sequences},
            ' ' * 4))
        linesList.append('>')
        return '\n'.join(linesList)


    def __repr__(self):
        if not self._parsed:
            data = self._file
            if len(data) > 0x80:
                data = repr(data[:0x80]) + '...'
            else:
                data = repr(data)
            return f'{self.__class__.__name__}({data}, names={self._names})'

        return f'{self.__class__.__name__}.fromEventsAndSequences({self.events}, {self.sequences})'


class SSARSequence:
    """
    A sequence embedded in a SSAR.
    """
    def __init__(self, firstEvent, bankID=0, volume=127, channelPressure=64,
                 polyphonicPressure=50, play=0):

        self.firstEvent = firstEvent
        self.bankID = bankID
        self.volume = volume
        self.channelPressure = channelPressure
        self.polyphonicPressure = polyphonicPressure
        self.play = play

    def save(self):
        """
        Return all the parts of the SSEQ, in the same format as the
        arguments to __init__.
        """
        return (self.firstEvent,
                self.bankID,
                self.volume,
                self.channelPressure,
                self.polyphonicPressure,
                self.play)

    def __str__(self):
        fields = []
        fields.append(f'bank={self.bankID}')
        fields.append(f'volume={self.volume}')
        fields.append(f'play={self.play}')
        return f'<ssar-sequence {" ".join(fields)}>'

    def __repr__(self):
        fields = ', '.join(repr(n) for n in [self.firstEvent,
                                             self.bankID,
                                             self.volume,
                                             self.channelPressure,
                                             self.polyphonicPressure,
                                             self.play])
        return f'{self.__class__.__name__}({fields})'
