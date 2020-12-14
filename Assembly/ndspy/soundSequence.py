
import enum
import struct

from . import _common


class SSEQ:
    """
    A sequence. This is generally used for background music.
    """
    # When saving SDAT, two otherwise identical SSEQs will share data
    # only if their dataMergeOptimizationIDs are the same.
    # You can pretty safely ignore this.
    dataMergeOptimizationID = 0

    def __init__(self, file=None, unk02=0, bankID=0, volume=127,
                 channelPressure=64, polyphonicPressure=50, play=0):
        #        ^
        # Default values here are the most common ones in the NSMB SDAT.

        self.unk02 = unk02
        self.bankID = bankID
        self.volume = volume
        self.channelPressure = channelPressure
        self.polyphonicPressure = polyphonicPressure
        self.play = play

        if file is not None:
            if not file.startswith(b'SSEQ'):
                raise ValueError("Wrong magic (should be b'SSEQ', instead "
                                 f'found {file[:4]})')

            self._file = file
            self._parsed = False

            version, = struct.unpack_from('<H', file, 6)
            if version != 0x100:
                raise ValueError(f'Unsupported SSEQ version: {version}')

        else:

            self._events = []
            self._parsed = True


    @classmethod
    def fromEvents(cls, events, unk02=0, bankID=0, volume=127,
                   channelPressure=64, polyphonicPressure=50, play=0):
        obj = cls(unk02=0, bankID=0, volume=127,
                  channelPressure=64, polyphonicPressure=50, play=0)
        obj.events = events
        return obj


    @property
    def events(self):
        if not self._parsed:
            raise ValueError('You must parse the SSEQ with .parse() before you'
                             ' can access .events!')
        return self._events
    @events.setter
    def events(self, value):
        self._events = value

    def parse(self):
        """
        Actually process the file.
        """
        self._events = []
        dataOffs, = struct.unpack_from('<I', self._file, 0x18)
        data = self._file[dataOffs:]
        self._events, _ = read_sequence_events(data, [])
        self._parsed = True

    def save(self):
        """
        Return all the parts of the SSEQ, in the same format as the
        arguments to __init__.
        """
        if self._parsed:
            seqEv, _ = save_sequence_events(self.events, [])

            file = bytearray(0x1C + len(seqEv))

            _common.NDS_STD_FILE_HEADER.pack_into(file, 0,
                b'SSEQ', 0xFEFF, 0x100, len(file), 0x10, 1)
            struct.pack_into('<4sII', file, 0x10,
                b'DATA', len(file) - 0x10, 0x1C)
            file[0x1C:] = seqEv
        else:
            file = self._file
        return (file,
                self.unk02,
                self.bankID,
                self.volume,
                self.channelPressure,
                self.polyphonicPressure,
                self.play)


    def __str__(self):
        fields = (f'bankID={self.bankID} volume={self.volume}'
                  f' channelPressure={self.channelPressure}'
                  f' polyphonicPressure={self.polyphonicPressure}'
                  f' play={self.play}')
        if not self._parsed:
            return f'<sseq unparsed {fields}>'

        linesList = [f'<sseq {fields}']
        linesList.append(_printSequenceEventList(
            self.events,
            {},
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
            return f'{self.__class__.__name__}({data})'

        return (f'{self.__class__.__name__}.fromEvents({self.events}'
                f'{self.unk02}, {self.bankID}, {self.volume}'
                f', {self.channelPressure}, {self.polyphonicPressure}, {self.play})')


class SequenceEvent:
    """
    Base class for an event that can occur within a sequence.
    """
    data_length = 1
    def __init__(self, type):
        self.type = type
    def save(self, eventsToOffsets=None):
        return bytes([self.type])
    @classmethod
    def from_data(cls, type, data, startOffset):
        raise NotImplementedError('SequenceEvent subclasses that can '
            'load themselves from data without context can implement this.')
    def __str__(self):
        return f'<sequence event {hex(self.type)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({hex(self.type)})'


def _read_variable_length_int(data, startOffset, limit=4):
    """
    Read a variable-length integer (as SSEQ encodes them) from `data`
    beginning at `startOffset`, limiting the number of read bytes to
    `limit`.
    
    While the code below looks complicated, the method to read such an
    integer is simple:
    - Read a byte. AND it with 0x7F for the part relevant to the
      integer value.
    - If its MSB (that you just trimmed off) is set, left-shift the int
      value by 7, move on to the next byte and repeat.

    So you read 7 bits as a time for as long as the MSB continues to be
    set.
    """
    offset = startOffset
    value = data[offset] & 0x7F; offset += 1
    length = 0
    while data[offset - 1] & 0x80:
        value <<= 7
        value |= data[offset] & 0x7F; offset += 1; length += 1
        if length > limit:
            raise ValueError('Read variable-length int past its end')
    return value


def _length_of_variable_length_int(x):
    """
    Returns the length of a variable-length integer `x`, as encoded in
    SSEQ. See _read_variable_length_int() for a description of the format.
    This can be implemented more concisely, but I opted for readability.
    """
    if x < 0:
        raise ValueError(f'Cannot write a negative variable-length int: {x}')
    bits = x.bit_length()
    length = 0
    while bits > 0:
        length += 1
        bits -= 7
    return max(1, length)


def _write_variable_length_int(x):
    """
    Find the bytes representing the arbitrarily-large (positive) integer
    `x` in the format used by SSEQ variable-length integer fields.
    See _read_variable_length_int() for a description of this format.
    """
    if x < 0:
        raise ValueError(f'Cannot write a negative variable-length int: {x}')

    ret = []

    while x:
        value = x & 0x7F
        x >>= 7
        ret.insert(0, value)

    for i, v in enumerate(ret[:-1]):
        ret[i] = v | 0x80

    return bytes(ret)


class NoteSequenceEvent(SequenceEvent):
    """
    Sequence events 0x00-0x7F: play a note.
    """
    @property
    def name(self):
        return _common.noteName(self.type)
    @property
    def data_length(self):
        return 2 + _length_of_variable_length_int(self.duration)
    def __init__(self, type, velocity, duration):
        super().__init__(type)
        self.velocity = velocity
        self.duration = duration
    def save(self, eventsToOffsets=None):
        return (super().save()
                + bytes([self.velocity])
                + _write_variable_length_int(self.duration))
    @classmethod
    def from_data(cls, type, data, startOffset):
        velocity = data[startOffset + 1]
        duration = _read_variable_length_int(data, startOffset + 2)
        return cls(type, velocity, duration)
    def __str__(self):
        return f'<{self.name} velocity={self.velocity} duration={self.duration}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.type}, {self.velocity}, {self.duration})'


class RestSequenceEvent(SequenceEvent):
    """
    Sequence event 0x80: a rest
    """
    @property
    def data_length(self):
        return 1 + _length_of_variable_length_int(self.duration)
    def __init__(self, duration):
        super().__init__(0x80)
        self.duration = duration
    def save(self, eventsToOffsets=None):
        return super().save() + _write_variable_length_int(self.duration)
    @classmethod
    def from_data(cls, type, data, startOffset):
        duration = _read_variable_length_int(data, startOffset + 1)
        return cls(duration)
    def __str__(self):
        return f'<rest {self.duration}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.duration})'


class InstrumentSwitchSequenceEvent(SequenceEvent):
    """
    Sequence event 0x81: switch to a different instrument
    """
    @property
    def data_length(self):
        return 1 + _length_of_variable_length_int(
            self.bankID << 7 | self.instrumentID)
    def __init__(self, bankID, instrumentID):
        super().__init__(0x81)
        self.bankID = bankID
        self.instrumentID = instrumentID
    def save(self, eventsToOffsets=None):
        value = self.instrumentID & 0x7F | self.bankID << 7
        return super().save() + _write_variable_length_int(value)
    @classmethod
    def from_data(cls, type, data, startOffset):
        value = _read_variable_length_int(data, startOffset + 1)
        instrumentID = value & 0x7F
        bankID = value >> 7
        return cls(bankID, instrumentID)
    def __str__(self):
        return f'<instrument {self.bankID}/{self.instrumentID}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.bankID}, {self.instrumentID})'


class BeginTrackSequenceEvent(SequenceEvent):
    """
    Sequence event 0x93: begin a track
    """
    data_length = 5
    def __init__(self, trackNumber, firstEvent):
        super().__init__(0x93)
        self.trackNumber = trackNumber
        self.firstEvent = firstEvent
    def save(self, eventsToOffsets=None):
        return (super().save()
                + bytes([self.trackNumber])
                + struct.pack('<I', eventsToOffsets[self.firstEvent])[:3])
    def __str__(self):
        return f'<begin track {self.trackNumber} id={id(self.firstEvent)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.trackNumber}, {self.firstEvent!r} at {id(self.firstEvent)})'


class JumpSequenceEvent(SequenceEvent):
    """
    Sequence event 0x94: a "jump" command
    """
    data_length = 4
    def __init__(self, destination):
        super().__init__(0x94)
        self.destination = destination
    def save(self, eventsToOffsets=None):
        return (super().save()
                + struct.pack('<I', eventsToOffsets[self.destination])[:3])
    def __str__(self):
        return f'<jump id={id(self.destination)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.destination!r} at {id(self.destination)})'


class CallSequenceEvent(SequenceEvent):
    """
    Sequence event 0x95: like a "jump" command, except that execution
    will return here when a ReturnSequenceEvent is encountered.
    """
    data_length = 4
    def __init__(self, destination):
        super().__init__(0x95)
        self.destination = destination
    def save(self, eventsToOffsets=None):
        return (super().save()
                + struct.pack('<I', eventsToOffsets[self.destination])[:3])
    def __str__(self):
        return f'<call id={id(self.destination)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.destination!r} at {id(self.destination)})'


class RandomSequenceEvent(SequenceEvent):
    """
    Sequence event 0xA0: Event with randomized argument.
    The first argument is the ID of the event you want to perform.
    This is followed by every argument to that (sub-)event except the
    last one. This is finally followed by a 2-byte "minimum" value and
    a 2-byte "maximum" value that will be used to choose a random value
    for the last argument to the sub-event.

    Disclaimer: this information is based purely on some variable names
    from sseq2mid and studying examples in a hex editor. I haven't
    actually tested this.
    """
    @staticmethod
    def _data_length_for_subtype(subType):
        if subType <= 0x7F:
            return 7

        # This is really really hacky, but... works
        for i in range(5):
            try:
                subEvent = _event_types[subType](0, *([0] * i))
                return 4 + subEvent.data_length
            except TypeError as e:
                pass
        raise ValueError("Couldn't determine data_length for"
            ' RandomSequenceEvent for some reason...?'
            f' subType={hex(subType)}')
    @property
    def data_length(self):
        return self._data_length_for_subtype(self.subType)
    def __init__(self, subType, args, randMin, randMax):
        super().__init__(0xA0)
        self.subType = subType
        self.args = args
        self.randMin = randMin
        self.randMax = randMax
    def save(self, eventsToOffsets=None):
        return (super().save()
                + bytes([self.subType, *self.args])
                + struct.pack('<hh', self.randMin, self.randMax))
    @classmethod
    def from_data(cls, type, data, startOffset):
        subType = data[startOffset + 1]
        length = cls._data_length_for_subtype(subType)

        args = []
        offset = startOffset + 2
        for i in range(length - 6):
            args.append(data[offset])
            offset += 1

        randMin, randMax = struct.unpack_from('<hh', data, offset)

        return cls(subType, args, randMin, randMax)

    def __str__(self):
        return (f'<random {hex(self.subType)}'
                f' {" ".join([str(x) for x in self.args])}'
                f'{" " if self.args else ""}'
                f'{(self.randMin, self.randMax)}>')
    def __repr__(self):
        return f'{self.__class__.__name__}({hex(self.subType)}, {self.args}, {self.randMin}, {self.randMax})'


class FromVariableSequenceEvent(SequenceEvent):
    """
    Sequence event 0xA1: Event with argument from a variable.
    This works something like event 0xA0 (RandomSequenceEvent), but I'm
    not sure why the size is smaller than you'd expect. Sorry. :/
    """
    @property
    def data_length(self):
        # Original comment on this logic from sseq2mid.c:
        # /* loveemu is a lazy person :P */

        # (loveemu is the author of sseq2mid.)
        # Interpret as you will.

        return 4 if (0xB0 <= self.subType <= 0xBD) else 3
    def __init__(self, subType, variableID, unknown=None):
        super().__init__(0xA1)
        self.subType = subType
        self.variableID = variableID
        self.unknown = unknown
    def save(self, eventsToOffsets=None):
        if self.data_length == 3:
            return super().save() + struct.pack('<Bh',
                                                self.subType,
                                                self.variableID)
        else:
            return super().save() + struct.pack('<Bbbxx',
                                                self.subType,
                                                self.unknown,
                                                self.variableID)
    @classmethod
    def from_data(cls, type, data, startOffset):
        subType = data[startOffset + 1]
        if 0xB0 <= subType <= 0xBD:
            unknown, variableID = struct.unpack_from('<bb',
                                                     data,
                                                     startOffset + 2)
        else:
            unknown = None
            variableID, = struct.unpack_from('<h', data, startOffset + 2)
        return cls(subType, variableID, unknown)

    def __str__(self):
        unk = f' {self.unknown}' if self.unknown is not None else ''
        return f'<from variable {hex(self.subType)} {self.variableID}{unk}>'
    def __repr__(self):
        unk = f', {self.unknown}' if self.unknown is not None else ''
        return f'{self.__class__.__name__}({hex(self.subType)}, {self.variableID}{unk})'


class IfSequenceEvent(SequenceEvent):
    """
    Sequence event 0xA2: Causes the following sequence event to occur
    only if the conditional flag is set to "true."
    """
    def __init__(self):
        super().__init__(0xA2)
    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls()
    def __str__(self):
        return '<if>'
    def __repr__(self):
        return f'{self.__class__.__name__}()'


def _make_arithmetic_sequence_event_class(typeNum, symbol, name, description):
    """
    Helper function to make a SequenceEvent subclass for the arithmetic
    operations.
    """
    __doc__ = f'Sequence event 0x{hex(typeNum).upper()[2:]}: {description}'
    def __init__(self, variableID, value):
        SequenceEvent.__init__(self, typeNum)
        self.variableID = variableID
        self.value = value
    def save(self, eventsToOffsets=None):
        return (SequenceEvent.save(self)
                + struct.pack('<Bh', self.variableID, self.value))
    @classmethod
    def from_data(cls, type, data, startOffset):
        variableID, value = struct.unpack_from('<Bh', data, startOffset + 1)
        return cls(variableID, value)
    def __str__(self):
        return f'<(var {self.variableID}) {symbol.lower()} {self.value}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.variableID}, {self.value})'
    return type(name,
                (SequenceEvent,),
                {'__doc__': __doc__,
                 '__init__': __init__,
                 'save': save,
                 'from_data': from_data,
                 '__str__': __str__,
                 '__repr__': __repr__,
                 'data_length': 4})

VariableAssignmentSequenceEvent = _make_arithmetic_sequence_event_class(0xB0,
    '=', 'VariableAssignmentSequenceEvent', 'Sets a variable equal to a value.')

VariableAdditionSequenceEvent = _make_arithmetic_sequence_event_class(0xB1,
    '+=', 'VariableAdditionSequenceEvent', 'Increments a variable by a value.')

VariableSubtractionSequenceEvent = _make_arithmetic_sequence_event_class(0xB2,
    '-=', 'VariableSubtractionSequenceEvent', 'Decrements a variable by a value.')

VariableMultiplicationSequenceEvent = _make_arithmetic_sequence_event_class(0xB3,
    '*=', 'VariableMultiplicationSequenceEvent', 'Multiplies a variable by a value in-place.')

VariableDivisionSequenceEvent = _make_arithmetic_sequence_event_class(0xB4,
    '/=', 'VariableDivisionSequenceEvent', 'Divides a variable by a value in-place.')

VariableShiftSequenceEvent = _make_arithmetic_sequence_event_class(0xB5,
    '[Shift]', 'VariableShiftSequenceEvent',
    'Performs a "[Shift]" operation. What that means is unclear.')

VariableRandSequenceEvent = _make_arithmetic_sequence_event_class(0xB6,
    '[Rand]', 'VariableRandSequenceEvent',
    'Performs a "[Rand]" operation. What that means is unclear.')

VariableUnknownB7SequenceEvent = _make_arithmetic_sequence_event_class(0xB7,
    '[unknown op 0xB7]', 'VariableUnknownB7SequenceEvent',
    'Performs an undocumented operation on a variable.')

VariableEqualSequenceEvent = _make_arithmetic_sequence_event_class(0xB8,
    '==', 'VariableEqualSequenceEvent',
    'Sets the conditional flag to true if a variable is equal to a value, or else to false.')

VariableGreaterThanOrEqualSequenceEvent = _make_arithmetic_sequence_event_class(
    0xB9, '>=', 'VariableGreaterThanOrEqualSequenceEvent',
    'Sets the conditional flag to true if a variable is greater than or equal to a value, or else to false.')

VariableGreaterThanSequenceEvent = _make_arithmetic_sequence_event_class(0xBA,
    '>', 'VariableGreaterThanSequenceEvent',
    'Sets the conditional flag to true if a variable is greater than a value, or else to false.')

VariableLessThanOrEqualSequenceEvent = _make_arithmetic_sequence_event_class(0xBB,
    '<=', 'VariableLessThanOrEqualSequenceEvent',
    'Sets the conditional flag to true if a variable is less than or equal to a value, or else to false.')

VariableLessThanSequenceEvent = _make_arithmetic_sequence_event_class(0xBC,
    '<', 'VariableLessThanSequenceEvent',
    'Sets the conditional flag to true if a variable is less than a value, or else to false.')

VariableNotEqualSequenceEvent = _make_arithmetic_sequence_event_class(0xBD,
    '!=', 'VariableNotEqualSequenceEvent',
    'Sets the conditional flag to true if a variable is not equal to a value, or else to false.')


def _make_simple_sequence_event_class(typeNum, shortName, name, description):
    """
    Helper function to make a simple SequenceEvent subclass with one
    parameter.
    """
    __doc__ = f'Sequence event 0x{hex(typeNum).upper()[2:]}: {description}'
    def __init__(self, value):
        SequenceEvent.__init__(self, typeNum)
        self.value = value
    def save(self, eventsToOffsets=None):
        return SequenceEvent.save(self) + bytes([self.value])
    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])
    def __str__(self):
        return f'<{shortName.lower()} {self.value}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.value})'
    return type(name,
                (SequenceEvent,),
                {'__doc__': __doc__,
                 '__init__': __init__,
                 'save': save,
                 'from_data': from_data,
                 '__str__': __str__,
                 '__repr__': __repr__,
                 'data_length': 2})


PanSequenceEvent = _make_simple_sequence_event_class(0xC0,
    'Pan', 'PanSequenceEvent', '"set panning value."')

ChannelVolumeSequenceEvent = _make_simple_sequence_event_class(0xC1,
    'Channel volume', 'ChannelVolumeSequenceEvent',
    'set the volume of the channel.')

GlobalVolumeSequenceEvent = _make_simple_sequence_event_class(0xC2,
    'Global volume', 'GlobalVolumeSequenceEvent',
    'set the volume of the entire sequence.')

TransposeSequenceEvent = _make_simple_sequence_event_class(0xC3,
    'Transpose', 'TransposeSequenceEvent',
    '"transpose (channel coarse tuning)."')

PortamentoSequenceEvent = _make_simple_sequence_event_class(0xC4,
    'Portamento', 'PortamentoSequenceEvent', '"portamento."')

PortamentoRangeSequenceEvent = _make_simple_sequence_event_class(0xC5,
    'Portamento range', 'PortamentoRangeSequenceEvent', '"portamento range."')

TrackPrioritySequenceEvent = _make_simple_sequence_event_class(0xC6,
    'Track priority', 'TrackPrioritySequenceEvent', '"set track priority."')


class MonoPolySequenceEvent(SequenceEvent):
    """
    Sequence event 0xC7: Switch between mono or poly mode.
    In mono mode, the event after a note event will not be processed
    until the note's duration has finished. This is conceptually nice,
    but prevents multiple notes from being played simultaneously on the
    same track.
    In poly mode, the event after a note event will immediately be
    processed while the note begins to play. This lets you write chords
    and generally more complicated music, but you'll need to explicitly
    put rest sequence events between every set of notes that aren't
    played simultaneously.
    """
    data_length = 2

    class MonoPolySequenceEventValue(enum.IntEnum):
        Poly = 0
        Mono = 1

    def __init__(self, value):
        super().__init__(0xC7)
        self.value = self.MonoPolySequenceEventValue(value)

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.value])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def __str__(self):
        return '<mono>' if self.value else '<poly>'
    def __repr__(self):
        return f'{self.__class__.__name__}({str(self.value)})'


class TieSequenceEvent(SequenceEvent):
    """
    Sequence event 0xC8: "tie."
    """
    data_length = 2

    class TieSequenceEventValue(enum.IntEnum):
        Off = 0
        On = 1

    def __init__(self, value):
        super().__init__(0xC8)
        self.value = self.TieSequenceEventValue(value)

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.value])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def __str__(self):
        return f'<tie {"on" if self.value else "off"}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({str(self.value)})'


class PortamentoFromSequenceEvent(SequenceEvent):
    """
    Sequence event 0xC9: set the current portamento pitch. The next note
    played will bend from this pitch to its own pitch over the note's
    entire duration. ...or something like that. That's the indication
    based on some SSARs I looked at (SAR_VS_COMMON_MENU in NSMB), but
    experiments didn't seem to confirm it. FWIW, this is called
    "portamento control" by some sources.
    """
    data_length = 2
    def __init__(self, value):
        super().__init__(0xC9)
        self.value = value

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.value])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def __str__(self):
        return f'<portamento from {_common.noteName(self.value)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.value})'


VibratoDepthSequenceEvent = _make_simple_sequence_event_class(0xCA,
    'Vibrato depth', 'VibratoDepthSequenceEvent',
    'set the depth of a vibrato effect.')

VibratoSpeedSequenceEvent = _make_simple_sequence_event_class(0xCB,
    'Vibrato speed', 'VibratoSpeedSequenceEvent',
    'set the speed of a vibrato effect.')


class VibratoTypeSequenceEvent(SequenceEvent):
    """
    Sequence event 0xCC: set the type of a vibrato effect.
    """
    data_length = 2

    class VibratoTypeSequenceEventValue(enum.IntEnum):
        Pitch = 0
        Volume = 1
        Pan = 2

    def __init__(self, value):
        super().__init__(0xCC)
        self.value = self.VibratoTypeSequenceEventValue(value)

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.value])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def _valueName(self):
        return ['Pitch', 'Volume', 'Pan'][self.value]
    def __str__(self):
        return f'<vibrato type {self._valueName().lower()}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({str(self.value)})'


VibratoRangeSequenceEvent = _make_simple_sequence_event_class(0xCD,
    'Vibrato range', 'VibratoRangeSequenceEvent',
    'set the range of a vibrato effect.')


class PortamentoOnOffSequenceEvent(SequenceEvent):
    """
    Sequence event 0xCE: turn portamento on or off.
    """
    data_length = 2

    class PortamentoOnOffSequenceEventValue(enum.IntEnum):
        Off = 0
        On = 1

    def __init__(self, value):
        super().__init__(0xCE)
        self.value = self.PortamentoOnOffSequenceEventValue(value)

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.value])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def __str__(self):
        return f'<portamento {"on" if self.value else "off"}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({str(self.value)})'


PortamentoDurationSequenceEvent = _make_simple_sequence_event_class(0xCF,
    'Portamento duration', 'PortamentoDurationSequenceEvent',
    'set the duration of a portamento.')

AttackRateSequenceEvent = _make_simple_sequence_event_class(0xD0,
    'Attack rate', 'AttackRateSequenceEvent', 'set the attack rate.')

DecayRateSequenceEvent = _make_simple_sequence_event_class(0xD1,
    'Decay rate', 'DecayRateSequenceEvent', 'set the decay rate.')

SustainRateSequenceEvent = _make_simple_sequence_event_class(0xD2,
    'Sustain rate', 'SustainRateSequenceEvent', 'set the sustain rate.')

ReleaseRateSequenceEvent = _make_simple_sequence_event_class(0xD3,
    'Release rate', 'ReleaseRateSequenceEvent', 'set the release rate.')


class BeginLoopSequenceEvent(SequenceEvent):
    """
    Sequence event 0xD4: begin a loop. The end of the loop is marked by
    a EndLoopSequenceEvent.
    """
    data_length = 2
    def __init__(self, loopCount):
        super().__init__(0xD4)
        self.loopCount = loopCount

    def save(self, eventsToOffsets=None):
        return super().save() + bytes([self.loopCount])

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(data[startOffset + 1])

    def __str__(self):
        return f'<begin loop {self.loopCount}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.loopCount})'


ExpressionSequenceEvent = _make_simple_sequence_event_class(0xD5,
    'Expression', 'ExpressionSequenceEvent', '"expression."')

PrintVariableSequenceEvent = _make_simple_sequence_event_class(0xD6,
    'Print variable', 'PrintVariableSequenceEvent',
    '"print variable (unknown)."')

VibratoDelaySequenceEvent = _make_simple_sequence_event_class(0xE0,
    'Vibrato delay', 'VibratoDelaySequenceEvent',
    'set the delay before a vibrato begins.')


class TempoSequenceEvent(SequenceEvent):
    """
    Sequence event 0xE1: "set the sequence tempo."
    """
    data_length = 3
    def __init__(self, value):
        super().__init__(0xE1)
        self.value = value

    def save(self, eventsToOffsets=None):
        return super().save() + struct.pack('<H', self.value)

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(struct.unpack_from('<H', data, startOffset + 1)[0])

    def __str__(self):
        return f'<tempo {self.value}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.value})'


class SweepPitchSequenceEvent(SequenceEvent):
    """
    Sequence event 0xE3: "sweep pitch."
    """
    data_length = 3
    def __init__(self, value):
        super().__init__(0xE3)
        self.value = value

    def save(self, eventsToOffsets=None):
        return super().save() + struct.pack('<H', self.value)

    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls(struct.unpack_from('<H', data, startOffset + 1)[0])

    def __str__(self):
        return f'<sweep pitch {self.value}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.value})'


class EndLoopSequenceEvent(SequenceEvent):
    """
    Sequence event 0xFC: end a loop that was previously begun by a
    BeginLoopSequenceEvent.
    """
    def __init__(self):
        super().__init__(0xFC)
    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls()
    def __str__(self):
        return '<end loop>'
    def __repr__(self):
        return f'{self.__class__.__name__}()'


class ReturnSequenceEvent(SequenceEvent):
    """
    Sequence event 0xFD: return to the event following the most recently
    encountered CallSequenceEffect.
    """
    def __init__(self):
        super().__init__(0xFD)
    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls()
    def __str__(self):
        return '<return>'
    def __repr__(self):
        return f'{self.__class__.__name__}()'


class DefineTracksSequenceEvent(SequenceEvent):
    """
    Sequence event 0xFE: define the tracks that will be used, and the
    event each begins with.
    """
    data_length = 3
    def __init__(self, trackNumbers):
        super().__init__(0xFE)
        self.trackNumbers = trackNumbers
    def save(self, eventsToOffsets=None):
        tracksBitfield = 0
        for i in range(16):
            if i in self.trackNumbers:
                tracksBitfield |= 1 << i
        return super().save() + struct.pack('<H', tracksBitfield)
    @classmethod
    def from_data(cls, type, data, startOffset):
        tracksBitfield, = struct.unpack_from('<H', data, startOffset + 1)
        trackNumbers = []
        for i in range(16):
            if tracksBitfield & (1 << i):
                trackNumbers.append(i)
        return cls(trackNumbers)
    def __str__(self):
        return f'<define tracks {" ".join(str(x) for x in self.trackNumbers)}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.trackNumbers})'


class EndTrackSequenceEvent(SequenceEvent):
    """
    Sequence event 0xFF: end the current track.
    """
    def __init__(self):
        super().__init__(0xFF)
    @classmethod
    def from_data(cls, type, data, startOffset):
        return cls()
    def __str__(self):
        return '<end track>'
    def __repr__(self):
        return f'{self.__class__.__name__}()'


class RawDataSequenceEvent(SequenceEvent):
    """
    A dummy "sequence event" that represents raw, unconsumed data.
    """
    @property
    def data_length(self):
        return len(self.data)
    def __init__(self, data=b''):
        super().__init__(None)
        self.data = data
    def save(self, eventsToOffsets=None):
        return self.data
    def __str__(self):
        return f'<raw data {self.data}>'
    def __repr__(self):
        return f'{self.__class__.__name__}({self.data})'


_event_types = {
    0x80: RestSequenceEvent,
    0x81: InstrumentSwitchSequenceEvent,
    0x93: BeginTrackSequenceEvent,
    0x94: JumpSequenceEvent,
    0x95: CallSequenceEvent,
    0xA0: RandomSequenceEvent,
    0xA1: FromVariableSequenceEvent,
    0xA2: IfSequenceEvent,
    0xB0: VariableAssignmentSequenceEvent,
    0xB1: VariableAdditionSequenceEvent,
    0xB2: VariableSubtractionSequenceEvent,
    0xB3: VariableMultiplicationSequenceEvent,
    0xB4: VariableDivisionSequenceEvent,
    0xB5: VariableShiftSequenceEvent,
    0xB6: VariableRandSequenceEvent,
    0xB7: VariableUnknownB7SequenceEvent,
    0xB8: VariableEqualSequenceEvent,
    0xB9: VariableGreaterThanOrEqualSequenceEvent,
    0xBA: VariableGreaterThanSequenceEvent,
    0xBB: VariableLessThanOrEqualSequenceEvent,
    0xBC: VariableLessThanSequenceEvent,
    0xBD: VariableNotEqualSequenceEvent,
    0xC0: PanSequenceEvent,
    0xC1: ChannelVolumeSequenceEvent,
    0xC2: GlobalVolumeSequenceEvent,
    0xC3: TransposeSequenceEvent,
    0xC4: PortamentoSequenceEvent,
    0xC5: PortamentoRangeSequenceEvent,
    0xC6: TrackPrioritySequenceEvent,
    0xC7: MonoPolySequenceEvent,
    0xC8: TieSequenceEvent,
    0xC9: PortamentoFromSequenceEvent,
    0xCA: VibratoDepthSequenceEvent,
    0xCB: VibratoSpeedSequenceEvent,
    0xCC: VibratoTypeSequenceEvent,
    0xCD: VibratoRangeSequenceEvent,
    0xCE: PortamentoOnOffSequenceEvent,
    0xCF: PortamentoDurationSequenceEvent,
    0xD0: AttackRateSequenceEvent,
    0xD1: DecayRateSequenceEvent,
    0xD2: SustainRateSequenceEvent,
    0xD3: ReleaseRateSequenceEvent,
    0xD4: BeginLoopSequenceEvent,
    0xD5: ExpressionSequenceEvent,
    0xD6: PrintVariableSequenceEvent,
    0xE0: VibratoDelaySequenceEvent,
    0xE1: TempoSequenceEvent,
    0xE3: SweepPitchSequenceEvent,
    0xFC: EndLoopSequenceEvent,
    0xFD: ReturnSequenceEvent,
    0xFE: DefineTracksSequenceEvent,
    0xFF: EndTrackSequenceEvent,
}


def read_sequence_events(data, notableOffsets):
    """
    Convert data containing sequence events (as seen in SSEQ and SSAR)
    to a list of SequenceEvent objects. A second list will also be
    returned that contains the elements from the first list that
    appeared at the offsets in notableOffsets. (This is useful if you
    have multiple starting positions that you want to keep track of, as
    is the case with SSAR.)
    """

    events = {}

    FATE_INPROGRESS = 0
    FATE_RETURN = 1
    FATE_LOOP = 2
    FATE_EOT = 3
    fates = {}

    def parse_at(off):
        offsetsOfMySequentialEvents = []

        while off < len(data):
            if off in fates:
                fate = fates[off]
                if fate == FATE_INPROGRESS:
                    fate = FATE_LOOP
                for off_ in offsetsOfMySequentialEvents:
                    fates[off_] = fate
                return fate

            try:

                type = data[off]

                if type == 0x93: # BeginTrack
                    trackNumber = data[off + 1]
                    firstEventOff, = struct.unpack_from('<I', data, off + 1)
                    firstEventOff >>= 8

                    event = BeginTrackSequenceEvent(trackNumber, None)
                    events[off] = event
                    fates[off] = FATE_INPROGRESS
                    parse_at(firstEventOff)
                    event.firstEvent = events[firstEventOff]

                elif type == 0x94: # Jump
                    destination, = struct.unpack_from('<I', data, off)
                    destination >>= 8

                    event = JumpSequenceEvent(None)
                    events[off] = event
                    fates[off] = FATE_INPROGRESS
                    fate = parse_at(destination)
                    event.destination = events[destination]

                    for off_ in offsetsOfMySequentialEvents:
                        fates[off_] = fate

                    # Should we keep parsing past here? Only if this
                    # is part of an if statement (and thus might be
                    # skipped).
                    x = off - 1
                    while x not in events and x >= 0:
                        x -= 1
                    if x == -1:
                        partOfIfStatement = False
                    else:
                        partOfIfStatement = (events[x].type == 0xA2)

                    if not partOfIfStatement:
                        return fate

                elif type == 0x95: # Call
                    destination, = struct.unpack_from('<I', data, off)
                    destination >>= 8

                    event = CallSequenceEvent(None)
                    events[off] = event
                    fates[off] = FATE_INPROGRESS
                    fate = parse_at(destination)
                    event.destination = events[destination]

                    if fate == FATE_EOT:
                        fates[off] = fate
                        for off_ in offsetsOfMySequentialEvents:
                            fates[off_] = fate
                        return fate
                    elif fate == FATE_RETURN:
                        pass
                    elif fate == FATE_LOOP:
                        fates[off] = fate
                        for off_ in offsetsOfMySequentialEvents:
                            fates[off_] = fate
                        return fate

                elif type == 0xFD: # Return
                    events[off] = ReturnSequenceEvent()
                    fates[off] = FATE_RETURN
                    for off_ in offsetsOfMySequentialEvents:
                        fates[off_] = FATE_RETURN
                    return FATE_RETURN

                elif type == 0xFF: # EoT
                    events[off] = EndTrackSequenceEvent()
                    fates[off] = FATE_EOT
                    for off_ in offsetsOfMySequentialEvents:
                        fates[off_] = FATE_EOT
                    return FATE_EOT

                else:

                    if type <= 0x7F:
                        eventCls = NoteSequenceEvent
                    elif type not in _event_types:
                        raise ValueError(f'Event {hex(type)} unrecognized.')
                    else:
                        eventCls = _event_types[type]

                    event = eventCls.from_data(type, data, off)
                    events[off] = event
                    fates[off] = FATE_INPROGRESS

                offsetsOfMySequentialEvents.append(off)
                off += event.data_length

            except (struct.error, IndexError):
                raise EOFError('Reached EoF of sequence.')
        raise EOFError('Reached EoF of sequence.')

    starts = notableOffsets
    if not starts: starts = [0]
    for start in starts:
        ultimateFate = parse_at(start)
        assert ultimateFate in (FATE_EOT, FATE_LOOP)

    eventsList = []
    i = 0
    while i < len(data):
        if i in events:
            eventsList.append(events[i])
            i += events[i].data_length
        else:
            j = i
            while j not in events and j < len(data):
                j += 1
            eventsList.append(RawDataSequenceEvent(data[i:j]))
            i = j

    notableEvents = [events[off] for off in notableOffsets]

    return eventsList, notableEvents


def save_sequence_events(events, notableEvents):
    """
    Convert a list of SequenceEvents to sequence event data. A list will
    also be returned that contains the offsets to which the events in
    notableEvents were saved. (The elements of notableEvents must
    therefore all be in events.)
    """
    
    events2Offsets = {}

    off = 0
    for e in events:
        events2Offsets[e] = off
        off += e.data_length
    data = bytearray(off)

    for event, offset in events2Offsets.items():
        eData = event.save(events2Offsets)
        data[offset : offset + len(eData)] = eData

    notableOffsets = [events2Offsets[e] for e in notableEvents]

    return data, notableOffsets


def _printSequenceEventList(events, labels, linePrefix):
    """
    Make a string representing the given list of sequence events.
    labels should be a dictionary: {name: event, name: event, ...}
    linePrefix will be prepended to every line.
    """
    linesList = []

    maxNameLen = 0
    i2SfxName = {}
    for name, event in labels.items():
        if event in events:
            i = events.index(event)
        elif event is None:
            linesList.append(f'{linePrefix}{name}: (none)')
            continue
        else:
            linesList.append(f'{linePrefix}(WARNING: {event!r} ({name}) not in .events!!)')
            continue
        if name is None: name = str(i)
        if i not in i2SfxName:
            i2SfxName[i] = name
        else:
            i2SfxName[i] += ', ' + name
        maxNameLen = max(maxNameLen, len(i2SfxName[i]))
    maxNameLen = min(maxNameLen, 48)
    maxNumLen = len(str(len(events) - 1))

    maxNameLen += 1 # for the ":" at the end

    def printEvent(e):
        label = i2SfxName[i] + ':' if i in i2SfxName else ''
        labelLst = []
        while label:
            labelLst.append(label[:maxNameLen])
            label = label[maxNameLen:]
        if labelLst:
            labelLst[-1] = f'{labelLst[-1]:{maxNameLen}}'
        else:
            labelLst = [' ' * maxNameLen]
        label = f'\n{linePrefix}'.join(labelLst)
        if isinstance(e, JumpSequenceEvent):
            etext = f'<jump @{events.index(e.destination)}>'
        elif isinstance(e, CallSequenceEvent):
            etext = f'<call @{events.index(e.destination)}>'
        elif isinstance(e, BeginTrackSequenceEvent):
            etext = f'<begin track {e.trackNumber} @{events.index(e.firstEvent)}>'
        else:
            etext = str(e)
        return f'{linePrefix}{label}  [{i:{maxNumLen}}] {etext},'

    for i, e in enumerate(events):
        linesList.append(printEvent(e))

    if events:
        linesList[-1] = linesList[-1][:-1] # remove last ","

    return '\n'.join(linesList)
