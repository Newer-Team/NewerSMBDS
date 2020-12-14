
class SequencePlayer:
    """
    Defines some stuff about sequences.
    """
    def __init__(self, maxSequences=0, channels=None, heapSize=0):
        self.maxSequences = maxSequences
        self.channels = channels if channels is not None else []
        self.heapSize = heapSize

    def save(self):
        return (self.maxSequences, self.channels, self.heapSize)
