
class STRM:
    """
    A streamed sound.
    """
    # When saving SDAT, two otherwise identical STRMs will share data
    # only if their dataMergeOptimizationIDs are the same.
    # You can pretty safely ignore this.
    dataMergeOptimizationID = 0

    def __init__(self, file=None, unk02=None, volume=0, priority=0, play=0, unk07=0):
        # The SDAT I'm using has no STRMs. If you find one that does,
        # go ahead and update the default values above!

        self.unk02 = unk02
        self.volume = volume
        self.priority = priority
        self.play = play
        self.unk07 = unk07

        if file is not None:
            if not file.startswith(b'STRM'):
                raise ValueError("Wrong magic (should be b'STRM', instead"
                                 f' found {file[:4]})')

            # And do something with the file!
        self.fileTEMP = file

    def save(self):
        """
        Return all the parts of the STRM, in the same format as the
        arguments to __init__.
        """
        return (self.fileTEMP,
                self.unk02,
                self.volume,
                self.priority,
                self.play,
                self.unk07)
