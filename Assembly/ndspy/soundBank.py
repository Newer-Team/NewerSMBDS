
import struct

from . import _common


class NoteDefinition:
    """
    A note definition in a SBNK Instrument.
    """
    def __init__(self, swavID=0, swarID=0, note=0, attack=0, decay=0,
            sustain=0, release=0, pan=0, unknown=1):
        self.swavID = swavID
        self.swarID = swarID
        self.note = note
        self.attack = attack
        self.decay = decay
        self.sustain = sustain
        self.release = release
        self.pan = pan
        self.unknown = unknown

    @classmethod
    def from_data(cls, data):
        return cls(*struct.unpack_from('<HH6B', data), 1)

    @classmethod
    def from_data_with_unknown(cls, data):
        values = struct.unpack_from('<3H6B', data)
        return cls(*values[1:], values[0])

    def save(self):
        return struct.pack('<HH6B', self.swavID, self.swarID,
            self.note, self.attack, self.decay, self.sustain,
            self.release, self.pan)

    def save_with_unknown(self):
        return struct.pack('<H', self.unknown) + self.save()

    def __str__(self):
        name = _common.noteName(self.note)
        return f'<note def {name} (SWAR {self.swarID}, SWAV {self.swavID})>'

    def __repr__(self):
        params = ', '.join(str(s) for s in
            [self.swavID, self.swarID, self.note, self.attack,
            self.decay, self.sustain, self.release, self.pan,
            self.unknown])
        return f'{self.__class__.__name__}({params})'
        


class Instrument:
    """
    A instrument in a SBNK.
    """
    # This attribute has to do with the way instrument data structs are
    # sorted within the SBNK. The data structs are always primarily
    # sorted by instrument type (first types < 16, then type 16, then
    # type 17). Within each of those three groups, though, the order is
    # arbitrary. Thus, this key allows you to set up whatever
    # arrangement you want.
    # It's extremely unlikely that you'll ever need to touch this, since
    # the order of structs doesn't really affect anything.
    bankOrderKey = 0

    dataMergeOptimizationID = 0

    def __init__(self, type):
        """
        Initialize the instrument.
        """
        self.type = type

    @classmethod
    def from_data(cls, type, data, startOffset):
        """
        Override this in subclasses.
        """
        raise NotImplementedError('Override from_data() in Instrument '
                                  'subclasses!')

    def save(self):
        """
        Just return the type of the instrument.
        """
        return (self.type,)


class SingleNoteInstrument(Instrument):
    """
    Instrument types 1-15: a single note definition.
    """
    def __init__(self, type, noteDefinition):
        super().__init__(type)
        self.noteDefinition = noteDefinition

    @classmethod
    def from_data(cls, type, data, startOffset):

        noteData = data[startOffset : startOffset + 10]
        noteDefinition = NoteDefinition.from_data(noteData)

        return cls(type, noteDefinition), 10

    def save(self):
        """
        Return (type, instrumentData)
        """
        return super().save() + (self.noteDefinition.save(),)

    def __str__(self):
        return f'<single-note instrument {self.noteDefinition}>'

    def __repr__(self):
        return f'{self.__class__.__name__}({self.type}, {self.noteDefinition!r})'


class RangeInstrument(Instrument):
    """
    Instrument type 16: a range of note definitions.
    """
    def __init__(self, firstNoteNumber, lastNoteNumber, noteDefinitions):
        super().__init__(16)
        self.firstNoteNumber = firstNoteNumber
        self.lastNoteNumber = lastNoteNumber
        self.noteDefinitions = noteDefinitions

    @classmethod
    def from_data(cls, _, data, startOffset):
        firstNoteNumber, lastNoteNumber = \
            struct.unpack_from('<BB', data, startOffset)

        noteDefinitions = []
        off = startOffset + 2
        for i in range(lastNoteNumber - firstNoteNumber + 1):
            noteData = data[off : off+0xC]
            noteDefinitions.append(
                NoteDefinition.from_data_with_unknown(noteData))
            off += 0xC

        return cls(firstNoteNumber, lastNoteNumber, noteDefinitions), off - startOffset

    def save(self):
        data = bytearray(2 + 0xC * len(self.noteDefinitions))
        struct.pack_into('<BB', data, 0,
            self.firstNoteNumber, self.lastNoteNumber)

        off = 2
        for n in self.noteDefinitions:
            data[off : off+0xC] = n.save_with_unknown()
            off += 0xC

        return super().save() + (data,)

    def __str__(self):
        joinList = []
        for i, d in enumerate(self.noteDefinitions):
            joinList.append(f'{_common.noteName(i + self.firstNoteNumber)}: {d}')
        defs = ', '.join(joinList)
        firstNote = _common.noteName(self.firstNoteNumber)
        lastNote = _common.noteName(self.lastNoteNumber)
        return f'<range instrument {firstNote}-{lastNote} {{{defs}}}>'

    def __repr__(self):
        defs = ', '.join(repr(d) for d in self.noteDefinitions)
        return f'{self.__class__.__name__}({self.firstNoteNumber}, {self.lastNoteNumber}, [{defs}])'


class RegionalInstrument(Instrument):
    """
    Instrument type 17: a instrument defining notes by a set of
    regions.
    """

    class Region:
        def __init__(self, endNote, noteDefinition):
            self.endNote = endNote
            self.noteDefinition = noteDefinition

        def __str__(self):
            return f'<region through {_common.noteName(self.endNote)} {self.noteDefinition}>'

        def __repr__(self):
            return f'{self.__class__.__name__}({self.endNote}, {self.noteDefinition!r})'

    # RegionalInstrument().regions is a list of [Region, Region, ...]

    def __init__(self, regions):
        super().__init__(17)
        self.regions = regions

    @classmethod
    def from_data(cls, _, data, startOffset):
        regionEnds = struct.unpack_from('8B', data, startOffset)

        regions = []
        off = startOffset + 8
        for e in regionEnds:
            if e == 0 and off != startOffset + 8: break

            noteData = data[off : off+0xC]
            note = NoteDefinition.from_data_with_unknown(noteData)
            off += 0xC

            regions.append(cls.Region(e, note))

        return cls(regions), off - startOffset

    def save(self):
        if len(self.regions) > 8:
            raise ValueError("Can't save RegionalInstrument -- too many "
                             f'regions ({len(self.regions)})!')

        data = bytearray(8 + 0xC * len(self.regions))
        for i, region in enumerate(self.regions):
            data[i] = region.endNote
            data[8 + 0xC * i : 0x14 + 0xC * i] = region.noteDefinition.save_with_unknown()

        return super().save() + (data,)

    def __str__(self):
        fill = []
        last = 0
        for region in self.regions:
            fill.append(f'{_common.noteName(last)}-{_common.noteName(region.endNote)}: {region.noteDefinition}')
            last = region.endNote + 1
        return f'<regional instrument {{{", ".join(fill)}}}>'

    def __repr__(self):
        return f'{self.__class__.__name__}([{", ".join(repr(r) for r in self.regions)}])'


def instrument_class(type):
    if type == 0:
        return None
    elif type < 16:
        return SingleNoteInstrument
    elif type == 16:
        return RangeInstrument
    elif type == 17:
        return RegionalInstrument
    else:
        raise ValueError(f'Instrument type {type} is invalid.')


def guessInstrumentType(data, startOffset, possibleTypes, bytesAvailable):
    """
    Try to guess the instrument type based on its data and a set of
    types that haven't been ruled out by its position in the surrounding
    data.
    None will be returned only if it's extremely unlikely that this is
    an instrument at all.
    """
    possibleTypes = set(possibleTypes)

    # By doing
    # if returnEarly(): return early()
    # you can quickly check if len(possibleTypes) < 2, and return the
    # appropriate type or None as appropriate.
    def returnEarly():
        return len(possibleTypes) < 2
    def early():
        if possibleTypes:
            return list(possibleTypes)[0]
        else:
            return None


    # Return immediately if possible
    if returnEarly(): return early()


    # Start by ruling out the impossible...
    def ruleOut(type):
        if type in possibleTypes:
            possibleTypes.remove(type)
    if bytesAvailable < 10:
        ruleOut(0)
    if bytesAvailable < 2 + 0xC:
        ruleOut(16)
    if bytesAvailable < 8 + 0xC:
        ruleOut(17)

    if returnEarly(): return early()


    # Then rule out the improbable...

    if 1 in possibleTypes: # single-note
        # SWAV ID and SWAR ID will probably never be > 0x400... right?
        if data[startOffset + 1] > 4: ruleOut(1)
        if data[startOffset + 3] > 4: ruleOut(1)
        # And note will probably never be 0, right?
        if data[startOffset + 4] == 0: ruleOut(1)
        # For that matter, if note is 0x3C (middle C), we can probably
        # just assume straight away that this is a single-note inst
        if data[startOffset + 4] == 0x3C: return 1
    if returnEarly(): return early()

    if 16 in possibleTypes: # range
        firstNote, lastNote = data[startOffset : startOffset+2]
        if firstNote > lastNote:
            ruleOut(16)
        else:
            expectedLen = 2 + 0xC * (lastNote - firstNote + 1)
            if expectedLen > bytesAvailable:
                ruleOut(16)
    if returnEarly(): return early()

    if 17 in possibleTypes: # regional
        regionEnds = data[startOffset : startOffset+8]

        # Check that all ends are strictly increasing, followed by
        # all zeroes
        # (not every regional instrument ends with 7F, surprisingly)
        previous = -1
        for end in regionEnds:
            if previous != 0 and end == 0:
                # first zero byte
                previous = 0
            elif previous == 0 and end != 0:
                # nonzero byte after a zero byte? bad bad bad
                ruleOut(17)
                break
            elif previous != 0:
                if end <= previous:
                    ruleOut(17)
                    break
                previous = end
        else:
            # Finally, check that we have sufficient length
            regionCount = 0
            for e in regionEnds:
                if not e: break
                regionCount += 1
            expectedLen = 8 + 0xC * regionCount
            if expectedLen > bytesAvailable:
                ruleOut(17)
    if returnEarly(): return early()


    # At this point, just pick one of the remaining options.
    return early()


class SBNK:
    """
    A bank. This defines a set of instruments that a sequence can use.
    """
    # When saving SDAT, two otherwise identical SBNKs will share data
    # only if their dataMergeOptimizationIDs are the same.
    # You can pretty safely ignore this.
    dataMergeOptimizationID = 0

    def __init__(self, file=None, unk02=0, waveArchives=None):
        self.unk02 = unk02
        if waveArchives is None:
            self.waveArchives = []
        else:
            self.waveArchives = list(waveArchives)
            while self.waveArchives and self.waveArchives[-1] is None:
                self.waveArchives = self.waveArchives[:-1]
        self.instruments = []
        self.inaccessibleInstruments = {}

        if file is not None:
            if not file.startswith(b'SBNK'):
                raise ValueError("Wrong magic (should be b'SBNK', instead "
                                 f'found {file[:4]})')

            self._init_from_data(file)


    @classmethod
    def fromInstruments(cls, instruments, unk02=0, waveArchives=None):
        obj = cls(unk02=unk02, waveArchives=waveArchives)
        obj.instruments = instruments
        return obj
    

    def _init_from_data(self, data):
        """
        Initialize the SBNK from file data.
        """

        magic, bom, version, filesize, headersize, numblocks = \
            _common.NDS_STD_FILE_HEADER.unpack_from(data, 0)
        assert magic == b'SBNK'
        if version != 0x100:
            raise ValueError(f'Unsupported SBNK version: {version}')

        dataMagic, dataSize, instrumentCount = \
            struct.unpack_from('<4sI32xI', data, 0x10)
        assert dataMagic == b'DATA'

        unconsumedBytes = set(range(0x3C + instrumentCount * 4, filesize))

        idsToOffsets = {}

        def makeInstrumentAt(type, offset):
            instrumentCls = instrument_class(type)
            if instrumentCls is None:
                instrument = None
                consumedBytesHere = 0
            else:
                instrument, consumedBytesHere = instrumentCls.from_data(type,
                                                    data,
                                                    offset)
                instrument.bankOrderKey = offset
                instrument.dataMergeOptimizationID = offset

            for j in range(consumedBytesHere):
                if offset + j in unconsumedBytes:
                    unconsumedBytes.remove(offset + j)

            return instrument

        dataArrayPos = 0x3C
        for id in range(instrumentCount):
            (instrumentType, instrumentOffset) = \
                struct.unpack_from('<BHx', data, dataArrayPos)
            dataArrayPos += 4

            instrument = makeInstrumentAt(instrumentType, instrumentOffset)
            self.instruments.append(instrument)

            if instrument is not None:
                idsToOffsets[id] = instrumentOffset

        # Why do we have to do this
        while unconsumedBytes:

            # Account for possible 2 bytes of padding at the end
            if (filesize - 1 in unconsumedBytes
                and filesize - 2 in unconsumedBytes
                and filesize - 3 not in unconsumedBytes):
                unconsumedBytes.remove(filesize - 1)
                unconsumedBytes.remove(filesize - 2)
            if not unconsumedBytes: break

            thisOffset = min(unconsumedBytes)

            # Find the previous and following instrument IDs
            prevID = -1
            prevOffset = -1
            nextID = 9E9
            nextOffset = 9E9
            for id, offset in idsToOffsets.items():
                if offset < thisOffset and offset > prevOffset:
                    prevID = id
                    prevOffset = offset
                if offset > thisOffset and offset < nextOffset:
                    nextID = id
                    nextOffset = offset
            if prevID == -1: prevID = None
            if nextID == 9E9: nextID = None

            # Find possible types for this instrument based on that
            possibleTypes = set([1, 16, 17])
            prevInst = None if prevID is None else self.instruments[prevID]
            nextInst = None if nextID is None else self.instruments[nextID]
            if prevInst is not None:
                if prevInst.type >= 16:
                    possibleTypes.remove(1)
                if prevInst.type == 17:
                    possibleTypes.remove(16)
            if nextInst is not None:
                if nextInst.type <= 16 and 17 in possibleTypes:
                    possibleTypes.remove(17)
                if nextInst.type < 16 and 16 in possibleTypes:
                    possibleTypes.remove(16)

            # Count the number of contiguous bytes we've got
            tempOffset = thisOffset + 1
            while tempOffset in unconsumedBytes:
                tempOffset += 1
            bytesAvailable = tempOffset - thisOffset

            # Use advanced heuristics™ to determine this instrument's
            # type
            instrumentType = guessInstrumentType(data, thisOffset, possibleTypes, bytesAvailable)

            if instrumentType is None:
                instrument = None
            else:
                try:
                    instrument = makeInstrumentAt(instrumentType, thisOffset)
                except Exception:
                    instrument = None

            if instrument is None:
                # Couldn't make an instrument there, so, assume that
                # it's garbage data that we have to skip (to avoid an
                # infinite loop)
                unconsumedBytes.remove(thisOffset)
                if thisOffset + 1 in unconsumedBytes:
                    unconsumedBytes.remove(thisOffset + 1)

            else:
                if prevID not in self.inaccessibleInstruments:
                    self.inaccessibleInstruments[prevID] = []
                self.inaccessibleInstruments[prevID].append(instrument)


    def save(self):
        """
        Return all the parts of the SBNK, in the same format as the
        arguments to __init__.
        """
        data = bytearray(0x3C + 4 * len(self.instruments))
        instrumentsData = bytearray()

        # Instrument data (that the offset points to) is stored in order of:
        # - Single-note instruments
        # - Range instruments
        # - Regional instruments
        # Yet the instruments offsets table is in the order that forms the
        # instruments IDs exposed to other files. So we save in two steps.
        # Also, the order within each of those three categories can be
        # arbitrary. So instruments have a bankOrderKey that can be used
        # to sort within each category.

        # First, put together the instruments data itself
        indexToOffset = [None for _ in self.instruments]
        dataReuseCache = {}
        def addInstrument(instrument):
            instrumentType, instrumentData = instrument.save()
            instrumentData = bytes(instrumentData)
            mergeID = instrument.dataMergeOptimizationID

            if (instrumentData, mergeID) not in dataReuseCache:
                dataReuseCache[(instrumentData, mergeID)] = len(instrumentsData)
                instrumentsData.extend(instrumentData)

            return dataReuseCache[(instrumentData, mergeID)]

        test1 = lambda id: id < 16
        test2 = lambda id: id == 16
        test3 = lambda id: id == 17
        if None in self.inaccessibleInstruments:
            for inacc in self.inaccessibleInstruments[None]:
                addInstrument(inacc)
        for test in (test1, test2, test3):
            inThisGroup = []
            for i, instrument in enumerate(self.instruments):
                if instrument is None: continue
                if test(instrument.type):
                    inThisGroup.append((i, instrument))

            inThisGroup.sort(key=lambda elem: elem[1].bankOrderKey)

            for i, instrument in inThisGroup:
                indexToOffset[i] = addInstrument(instrument)

                if i in self.inaccessibleInstruments:
                    for inacc in self.inaccessibleInstruments[i]:
                        addInstrument(inacc)

        # Add any inaccessible instruments with IDs that are too high
        idSet = set(self.inaccessibleInstruments)
        if None in idSet: idSet.remove(None)
        for inaccID in sorted(idSet):
            if inaccID < len(self.instruments): continue
            for inacc in self.inaccessibleInstruments[inaccID]:
                addInstrument(inacc)

        # And now construct the table using that.
        for i, instrument in enumerate(self.instruments):
            instrumentType = instrument.type if instrument is not None else 0

            offs = indexToOffset[i]
            if offs is None: offs = 0
            else: offs += len(data)

            struct.pack_into('<BH', data, 0x3C + 4 * i,
                             instrumentType, offs)

        data.extend(instrumentsData)

        while len(data) % 4:
            data.append(0)

        _common.NDS_STD_FILE_HEADER.pack_into(data, 0,
            b'SBNK', 0xFEFF, 0x100, len(data), 0x10, 1)
        struct.pack_into('<4sI32xI', data, 0x10,
            b'DATA', len(data) - 0x10, len(self.instruments))

        return (data, self.unk02, self.waveArchives)

    def __str__(self):
        linesList = [f'<sbnk waveArchives={self.waveArchives}']

        maxNumLen = len(str(len(self.instruments) - 1))

        for i, inst in enumerate(self.instruments):
            linesList.append(f'    [{i:{maxNumLen}}] {inst}')

        linesList.append('>')
        return '\n'.join(linesList)

    def __repr__(self):
        return f'{self.__class__.__name__}.fromInstruments({self.instruments}, waveArchives={self.waveArchives})'