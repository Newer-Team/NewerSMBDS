import struct

from . import _common
from . import codeCompression


class MainCodeFile:
    """
    Represents either arm7.bin or arm9.bin.
    """

    class Section:
        """
        arm9.bin and arm7.bin are split into sections that are placed at
        various RAM addresses. This represents one of those sections.
        """

        # There's an implicit first section in the file, which is not
        # defined in the sections table with the others. This attribute
        # will be True if this is that section.
        implicit = False


        def __init__(self, data, ramAddress, bssSize):
            self.data = bytearray(data)
            self.ramAddress = ramAddress
            self.bssSize = bssSize


    sections = None
    ramAddress = 0x00000000
    codeSettingsOffs = 0x00000000


    def __init__(self, data, ramAddress):
        self.sections = []
        self.ramAddress = ramAddress

        data = codeCompression.decompress(data)

        self.codeSettingsOffs = self._findCodeSettingsOffs(data)

        if self.codeSettingsOffs is not None:
            copyTableBegin, copyTableEnd, dataBegin = \
                struct.unpack_from('<3I', data, self.codeSettingsOffs)
            copyTableBegin -= ramAddress
            copyTableEnd -= ramAddress
            dataBegin -= ramAddress
        else:
            # The entire file is one implied section
            copyTableBegin = copyTableEnd = 0
            dataBegin = len(data)

        def makeSection(ramAddr, ramLen, fileOffs, bssSize):
            sdata = data[fileOffs : fileOffs + ramLen]
            self.sections.append(self.Section(sdata, ramAddr, bssSize))

        # Implicit first section
        makeSection(ramAddress, dataBegin, 0, 0)
        self.sections[0].implicit = True

        copyTablePos  = copyTableBegin
        while copyTablePos < copyTableEnd:
            secRamAddr, secSize, bssSize = \
                struct.unpack_from('<3I', data, copyTablePos)
            copyTablePos += 12

            makeSection(secRamAddr, secSize, dataBegin, bssSize)

            dataBegin += secSize


    @classmethod
    def fromCompressed(cls, data, *args):
        data = codeCompression.decompress(data)
        return cls(data, *args)


    def save(self, compress=False):
        """
        Save this code file as a bytes object.
        Note that ARM7 should never be compressed!
        """
        data = bytearray()

        for s in self.sections:
            data.extend(s.data)

            # Align to 0x04
            while len(data) % 4:
                data.append(0)

        # These loops are NOT identical!
        # The first one only operates on sections with length != 0,
        # and the second operates on sections with length == 0!

        sectionTable = bytearray()

        for s in self.sections:
            if s.implicit: continue
            if len(s.data) == 0: continue
            sectionTable.extend(
                struct.pack('<3I', s.ramAddress, len(s.data), s.bssSize))

        for s in self.sections:
            if s.implicit: continue
            if len(s.data) != 0: continue
            sectionTable.extend(
                struct.pack('<3I', s.ramAddress, len(s.data), s.bssSize))

        sectionTableOffset = len(data)
        data.extend(sectionTable)

        def setInt(addr, val):
            struct.pack_into('<I', data, addr, val)

        sectionTableAddr = self.ramAddress + sectionTableOffset
        sectionTableEnd = sectionTableAddr + len(sectionTable)

        cso = self.codeSettingsOffs
        if cso is not None:
            setInt(cso + 0x00, sectionTableAddr)
            setInt(cso + 0x04, sectionTableEnd)
            setInt(cso + 0x08, self.ramAddress + len(self.sections[0].data))
        else:
            # Welp, hopefully we only have one section :P
            pass

        if compress:
            data = bytearray(codeCompression.compress(data, True, True))
            setInt(cso + 0x14, self.ramAddress + len(data))
        else:
            setInt(cso + 0x14, 0)

        return data


    def _findCodeSettingsOffs(self, data):
        """
        Find the offset of the code settings area in the data given.
        """

        # Simple heuristic that works in most arm9.bin's:
        for i in range(0, 0x8000, 4):
            if data[i:i+8] == b'\x21\x06\xC0\xDE\xDE\xC0\x06\x21':
                return i - 0x1C

        # But to support arm7, which lacks that magic, we can fall back
        # to a different heuristic based on the assumption that the code
        # section table will be the very last thing in the code file.
        # Which... isn't a great assumption, but it's the best I've got
        # right now.
        expectedTableEnd = self.ramAddress + len(data)
        expectedTableEndBytes = struct.pack('<I', expectedTableEnd)
        if expectedTableEndBytes in data:
            match = data.index(expectedTableEndBytes, 0)
            while match is not None:
                possibleTableStart, = struct.unpack_from('<I', data, match - 4)
                if (possibleTableStart % 4 == 0
                        and (expectedTableEnd - possibleTableStart) % 12 == 0
                        and expectedTableEnd - possibleTableStart < 0x100):
                    # Probably a match
                    return match - 4
                match = data.index(expectedTableEndBytes, match + 1)


class Overlay:
    """
    Represents a code overlay.
    """

    data = None
    ramAddress = 0x00000000
    ramSize = 0
    bssSize = 0
    staticInitStart = 0x00000000
    staticInitEnd = 0x00000000
    fileID = 0
    compressedSize = 0
    flags = 0
    unkAddress = 0x00000000


    def __init__(self, data, ramAddress, ramSize, bssSize, staticInitStart,
            staticInitEnd, fileID, compressedSize, flags):
        self.data = bytearray(codeCompression.decompress(data))
        self.ramAddress = ramAddress
        self.ramSize = ramSize
        self.bssSize = bssSize
        self.staticInitStart = staticInitStart
        self.staticInitEnd = staticInitEnd
        self.fileID = fileID
        self.compressedSize = compressedSize
        self.flags = flags


    @property
    def compressed(self):
        return bool(self.flags & 1)
    @compressed.setter
    def compressed(self, value):
        if value:
            self.flags |= 1
        else:
            self.flags &= ~1


    @property
    def verifyHash(self):
        return bool(self.flags & 2)
    @verifyHash.setter
    def verifyHash(self, value):
        if value:
            self.flags |= 2
        else:
            self.flags &= ~2


    def save(self, compress=False):
        """
        Save the Overlay as a bytes object.
        This updates several attributes to match the output
        representation.
        """
        self.ramSize = len(self.data)
        if compress:
            data = codeCompression.compress(self.data, False, True)
        else:
            data = self.data
        self.compressedSize = len(data)
        self.compressed = compress
        return data


    def __str__(self):
        fields = []
        fields.append('at 0x%08X' % self.ramAddress)
        fields.append(f'file={self.fileID}')
        if self.compressed:
            fields.append('compressed')
        if self.verifyHash:
            fields.append('verify-hash')

        return f'<overlay {" ".join(fields)}>'

    def __repr__(self):
        fields = []
        if len(self.data) <= 0x10:
            fields.append(repr(bytes(self.data)))
        else:
            fields.append(repr(bytes(self.data[:0x10])) + '...')
        fields.append('0x%08X' % self.ramAddress)
        fields.append('0x%X' % self.ramSize)
        fields.append('0x%X' % self.bssSize)
        fields.append('0x%08X' % self.staticInitStart)
        fields.append('0x%08X' % self.staticInitEnd)
        fields.append(str(self.fileID))
        fields.append('0x%X' % self.compressedSize)
        fields.append('0x%X' % self.flags)
        return f'{self.__class__.__name__}({", ".join(fields)})'


def loadOverlayTable(tableData, fileCallback):
    """
    A helper function to load an overlay table.
    fileCallback should be a function that takes an integer representing
    an overlay ID and another representing a file ID, and returns a
    bytes object.
    """
    ovs = {}
    for i in range(0, len(tableData), 32):
        (ovID, ramAddr, ramSize, bssSize, staticInitStart, staticInitEnd,
            fileID, compressedSize_Flags) = struct.unpack_from('<8I', tableData, i)
        ovs[ovID] = Overlay(fileCallback(ovID, fileID), ramAddr, ramSize,
            bssSize, staticInitStart, staticInitEnd, fileID,
            compressedSize_Flags & 0xFFFFFF, compressedSize_Flags >> 24)
    return ovs


def saveOverlayTable(table):
    """
    Given a dict in the format of the return value of
    loadOverlayTable(), reconstruct overlay table data.
    """
    if not table: return b''

    ovt = bytearray()

    # Ensure that we loop over overlay IDs in order
    for ovId in sorted(table):
        ov = table[ovId]

        values = []
        values.append(ovId)                # 0x00
        values.append(ov.ramAddress)       # 0x04
        values.append(ov.ramSize)          # 0x08
        values.append(ov.bssSize)          # 0x0C
        values.append(ov.staticInitStart)  # 0x10
        values.append(ov.staticInitEnd)    # 0x14
        values.append(ov.fileID)           # 0x18
        values.append(ov.compressedSize | ov.flags << 24) # 0x1C

        ovt.extend(struct.pack('<8I', *values))

    return ovt