
import enum


class GroupEntryType(enum.IntEnum):
    SSEQ = 0
    SBNK = 1
    SWAR = 2
    SSAR = 3


class SWARLoadMethod(enum.IntEnum):
    fileIDs = 0
    SWARIDs = 1


class GroupEntry:
    """
    A single entry in a group.
    """

    def __init__(self, type=GroupEntryType.SSEQ, options=0, id=0):
        self.type = GroupEntryType(type)
        self.id = id

        # Observed values for "options" (in binary):
        # SSEQ: 0111, 0011, 0001 (, 1111 in "NNS_SndArcLoadSeq")
        # SBNK: 0110, 0010
        # SWAR: 0100
        # SSAR: 1000

        # The semantics below are based on actual code RE, though.

        # Note that a SSEQ will pass its "options" value on to the
        # SBNK it loads, and a SBNK will do the same with the SWARs it
        # loads.

        self.loadSSEQ = options & 1
        self.loadSBNKSWARsFrom = SWARLoadMethod(bool(options & 2))
        self.loadSWAR = options & 4
        self.loadSSAR = options & 8


    def save(self):
        """
        Return group entry info in the same format it was passed into
        the constructor.
        """

        options = 0
        if self.loadSSEQ: options |= 1
        if self.loadSBNKSWARsFrom == SWARLoadMethod.SWARIDs: options |= 2
        if self.loadSWAR: options |= 4
        if self.loadSSAR: options |= 8

        return self.type, options, self.id


    def __repr__(self):
        typeName = {
            GroupEntryType.SSEQ: 'sseq',
            GroupEntryType.SBNK: 'sbnk',
            GroupEntryType.SWAR: 'swar',
            GroupEntryType.SSAR: 'ssar',
            }[self.type]

        optionsL = []
        if self.loadSSEQ: optionsL.append('load-sseq')
        if self.loadSBNKSWARsFrom:
            method = {
                SWARLoadMethod.fileIDs: 'file-id',
                SWARLoadMethod.SWARIDs: 'swar-id',
                }[self.loadSBNKSWARsFrom]
            optionsL.append('load-swars-by-' + method)
        if self.loadSWAR: optionsL.append('load-swar')
        if self.loadSSAR: optionsL.append('load-ssar')
        options = ' '.join(optionsL)

        return f'<group-entry-{typeName} {self.id} {options}>'