"""
**BMG** files contain all of the text in a game that will be displayed to
the player (apart from text embedded into images). Games contain one or
more BMG files for each language they support, and will load the
appropriate one depending on the console's language setting. Each
"message" is referenced by index.

Some BMG files -- namely, those used in the DS Zelda games -- can
contain game flow scripts in addition to text. ndspy can read and save
the file blocks that contain these scripts (*FLW1* and *FLI1*), but
decoding and encoding the instructions themselves is out of its scope.
"""


import collections
import struct

from . import _common


class BMG:
    """
    A BMG file.

    :param data: The :py:class:`bytes` object to be read as BMG file
        data. If this is missing or ``None``, the
        :py:class:`BMG` object will initially be empty.

    .. py:attribute:: messages

        The list of :py:class:`Message`\ s in this BMG.

        :type: :py:class:`list` of :py:class:`Message`

    .. py:attribute:: instructions

        The script instructions in this BMG, if it has a *FLW1* block.
        Instructions will be :py:class:`bytes` objects by default, but
        when saving, any object that implements a ``.save() -> bytes``
        method is acceptable in place of :py:class:`bytes`. (This is to
        let you implement custom classes for instructions if you want
        to.)

        :type: :py:class:`list` of :py:class:`bytes` or objects
            implementing ``.save() -> bytes``

    .. py:attribute:: labels

        The script instruction labels in this BMG, if it has a *FLW1*
        block.

        :type: :py:class:`list` of ``(bmgID, instructionIndex)`` (both :py:class:`int`\ s)

    .. py:attribute:: scripts

        The starting instruction indices for each script ID defined in
        this BMG, if it has a *FLI1* block.

        :type: :py:class:`collections.OrderedDict`: ``{scriptID: instructionIndex}`` (both :py:class:`int`\ s)

    .. py:attribute:: id

        This BMG's ID number. In at least some games, every BMG has a
        unique ID. This makes it possible to refer to specific messages
        by specifying the desired BMG ID and the message index within
        that BMG.

        :type: :py:class:`int`
        :default: 0

    .. py:attribute:: unk10

        Unknown header value at 0x10.

        :type: :py:class:`int`
        :default: 2

    .. py:attribute:: unk14

        Unknown header value at 0x14.

        :type: :py:class:`int`
        :default: 0

    .. py:attribute:: unk18

        Unknown header value at 0x18.

        :type: :py:class:`int`
        :default: 0

    .. py:attribute:: unk1C

        Unknown header value at 0x1C.

        :type: :py:class:`int`
        :default: 0
    """

    def __init__(self, data=None):

        self.messages = []
        self.instructions = []
        self.labels = []
        self.scripts = collections.OrderedDict()

        self.id = 0

        self.unk10 = 2
        self.unk14 = 0
        self.unk18 = 0
        self.unk1C = 0

        if data is not None:
            self._initFromData(data)

    def _initFromData(self, data):
        if data[:8] != b'MESGbmg1':
            raise ValueError('Not a BMG file.')

        magic, dataLen, sectionCount, self.unk10, unk14, unk18, unk1C = \
            struct.unpack_from('<8s6I', data, 0)

        INF1 = []
        def parseINF1(offset, length):
            count, entryLength, self.id = struct.unpack_from('<HHI', data, offset + 8)
            assert entryLength == 8

            for i in range(count):
                entryOff, entryAttribs = struct.unpack_from('<II', data, offset + 16 + i * 8)
                INF1.append((entryOff, entryAttribs))

        DAT1 = b''
        def parseDAT1(offset, length):
            nonlocal DAT1
            DAT1 = data[offset + 8 : offset + length]

        self.instructions = []
        self.labels = []
        def parseFLW1(offset, length):
            instructionsCount, labelsCount, unk0C = \
                struct.unpack_from('<HHI', data, offset + 8)
            # unk0C is always 0, as far as I can tell

            instructionsTableOffset = offset + 16
            for i in range(instructionsCount):
                instOff = instructionsTableOffset + i * 8
                cmd = data[instOff : instOff + 8]
                if cmd != b'\0\0\0\0\0\0\0\0':
                    self.instructions.append(cmd)

            indicesTableOffset = instructionsTableOffset + instructionsCount * 8
            bmgIDsTableOffset = indicesTableOffset + labelsCount * 2
            for i in range(labelsCount):
                index, = struct.unpack_from('<h', data, indicesTableOffset + i * 2)
                bmgID, = struct.unpack_from('<b', data, bmgIDsTableOffset + i)
                if bmgID != 0 or index != 0:
                    self.labels.append((bmgID, index))

        self.scripts = collections.OrderedDict()
        def parseFLI1(offset, length):
            count, entryLength, unk0C = struct.unpack_from('<HHI', data, offset + 8)
            assert entryLength == 8
            # unk0C is always 0, as far as I can tell

            for i in range(count):
                id, index = struct.unpack_from('<IHxx', data, offset + 16 + i * 8)
                self.scripts[id] = index

        offset = 0x20
        for i in range(sectionCount):
            sectionMagic, sectionLen = struct.unpack_from('<4sI', data, offset)
            if sectionMagic == b'INF1':
                parseINF1(offset, sectionLen)
            elif sectionMagic == b'DAT1':
                parseDAT1(offset, sectionLen)
            elif sectionMagic == b'FLW1':
                parseFLW1(offset, sectionLen)
            elif sectionMagic == b'FLI1':
                parseFLI1(offset, sectionLen)
            else:
                raise ValueError('Unknown BMG section: ' + repr(sectionMagic))
            offset += sectionLen

        # Now we just need to read the messages.

        self.messages = []
        for offset, attribs in INF1:

            # The "currentStringStart" setup may seem needlessly
            # confusing, but it's intended to keep the number of calls
            # to bytes.decode() to a minimum. Based on my testing, it
            # really does make the code run significantly faster.

            stringParts = []
            currentStringStart = offset

            nextBytes = DAT1[offset : offset + 2]
            while nextBytes != b'\0\0':
                if nextBytes == b'\x1A\x00': # escape sequence
                    if currentStringStart and currentStringStart != offset:
                        stringParts.append(DAT1[currentStringStart:offset].decode('utf-16le'))
                    escapeLen, escapeType = DAT1[offset + 2 : offset + 4]
                    escapeData = DAT1[offset + 4 : offset + escapeLen]
                    stringParts.append(Message.Escape(escapeType, escapeData))
                    offset += escapeLen
                    currentStringStart = offset
                else:
                    offset += 2

                nextBytes = DAT1[offset : offset + 2]

            if currentStringStart and currentStringStart != offset:
                stringParts.append(DAT1[currentStringStart:offset].decode('utf-16le'))

            self.messages.append(Message(attribs, stringParts, offset == 0))


    def save(self):
        """
        Generate file data representing this BMG.

        *FLW1* and *FLI1* sections will be created only if any
        script instructions or scripts exist, respectively.

        :returns: The BMG file data.
        :rtype: :py:class:`bytes`
        """
        data = bytearray(0x20)

        instructionsCount = len(self.instructions)
        if instructionsCount % 2: instructionsCount += 1

        labelsCount = len(self.labels)
        while labelsCount % 8: labelsCount += 1

        numSections = 2
        if self.instructions: numSections += 1
        if self.scripts: numSections += 1

        INF1 = bytearray(16)
        DAT1 = bytearray(10) # Starts with 2 null bytes for whatever reason
        FLW1 = bytearray(16)
        FLI1 = bytearray(16)

        for message in self.messages:
            offset = 0 if message.isNull else len(DAT1) - 8
            INF1.extend(struct.pack('<II', offset, message.info))
            if not message.isNull:
                DAT1.extend(message.save())

        for inst in self.instructions:
            if hasattr(inst, 'save'):
                inst = inst.save()
            if len(inst) != 8:
                raise ValueError(f'Length of instruction {inst} is not 8 bytes!')
            FLW1.extend(inst)
        while len(FLW1) % 16: FLW1.extend(b'\0' * 8)

        for bmgID, instIndex in self.labels:
            FLW1.extend(struct.pack('<h', instIndex))
        for _ in range(labelsCount - len(self.labels)):
            FLW1.extend(b'\0\0')
        for bmgID, instIndex in self.labels:
            FLW1.extend(struct.pack('<b', bmgID))

        for id, startIndex in self.scripts.items():
            FLI1.extend(struct.pack('<II', id, startIndex))

        # Sections' lengths must be 32-byte aligned
        while len(INF1) % 32: INF1.append(0)
        while len(DAT1) % 32: DAT1.append(0)
        while len(FLW1) % 32: FLW1.append(0)

        # FLI1's length isn't actually padded, but the length it claims
        # in its header is. (I know. It's confusing.)
        FLI1len = len(FLI1)
        while FLI1len % 32: FLI1len += 1

        # Pack section headers
        struct.pack_into('<4sIHHI', INF1, 0,
            b'INF1', len(INF1), len(self.messages), 8, self.id)
        struct.pack_into('<4sI', DAT1, 0, b'DAT1', len(DAT1))
        if self.instructions:
            struct.pack_into('<4sIHH', FLW1, 0,
                b'FLW1', len(FLW1), instructionsCount, labelsCount)
        if self.scripts:
            struct.pack_into('<4sIHH', FLI1, 0,
                b'FLI1', FLI1len, len(self.scripts), 8)

        # Insert the sections
        data.extend(INF1)
        data.extend(DAT1)
        if self.instructions: data.extend(FLW1)
        if self.scripts: data.extend(FLI1)

        # Pack the BMG header
        totalLen = len(data)
        while totalLen % 32: totalLen += 1
        struct.pack_into('<8s3I', data, 0, b'MESGbmg1', totalLen, numSections, self.unk10)

        return bytes(data)


class Message:
    """
    A single message in a BMG file.

    BMG messages are more than simple strings; they contain escape
    sequences that can specify font formatting and allow text to be
    inserted at runtime. For this reason, the message data is
    represented as a list of strings and :py:class:`Escape`\ s instead
    of as a string.

    :param info: The initial value for the ``info`` attribute.

    :param stringParts: The initial value of the ``stringParts``
        attribute. If ``None``, ``stringParts`` will be initialized as
        an empty list.

    :param isNull: The initial value for the ``isNull`` attribute.


    .. py:attribute:: info

        A value representing message metadata, which comes from the
        BMG's *INF1* block. The specific meaning of this value is
        currently unclear.

        :type: :py:class:`int`
        :default: 0

    .. py:attribute:: stringParts

        A list of strings and escape sequences that together form the
        message. Empty strings are allowed but discouraged.

        :type: :py:class:`list` of :py:class:`str` and :py:class:`Escape`

    .. py:attribute:: isNull

        This is ``True`` if the message is null; that is, if its data
        offset value in *INF1* is 0.

        :type: :py:class:`bool`
        :default: ``False``
    """

    class Escape:
        """
        An escape sequence within a BMG message.

        Escape sequences have a type and optional parameter data.
        Currently, the parameter data is left raw and unparsed; this may
        change in the future.

        :param type: The initial value for the ``type`` attribute.

        :param data: The initial value of the ``data`` attribute.


        .. py:attribute:: type

            The type ID of this escape sequence.

            :type: :py:class:`int`
            :default: 0

        .. py:attribute:: data

            The raw data contained in this escape sequence.

            :type: :py:class:`bytes`
        """

        def __init__(self, type=0, data=b''):
            self.type = type
            self.data = data

            # Type = 4 is used for pluralization
            # (and there's a couple parameters -- need to look into that)

        def save(self):
            """
            Generate binary data representing this escape sequence.

            :returns: The escape sequence data.
            :rtype: :py:class:`bytes`
            """
            data = bytearray(b'\x1A\x00')
            data.append(len(self.data) + 4)
            data.append(self.type)
            data.extend(self.data)
            return data

        def __repr__(self):
            return f'{type(self).__name__}({self.type}, {self.data!r})'

        def __str__(self):
            return f'[{self.type}:{self.data.hex()}]'

    info = 0
    stringParts = []
    isNull = False

    def __init__(self, info=0, stringParts=None, isNull=False):
        self.info = info
        self.stringParts = [] if stringParts is None else stringParts
        self.isNull = isNull

    def save(self):
        """
        Generate binary data representing this message.

        :returns: The message data.
        :rtype: :py:class:`bytes`
        """
        data = bytearray()
        for part in self.stringParts:
            if isinstance(part, str):
                data.extend(part.encode('utf-16le'))
            else:
                data.extend(part.save())
        data.extend(b'\0\0')
        return data

    def __repr__(self):
        return f'{type(self).__name__}({hex(self.info)}, {self.stringParts})'

    def __str__(self):
        return ''.join(str(s) for s in self.stringParts)