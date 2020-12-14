
class StreamPlayer:
    """
    Defines some stuff about streams.
    """
    def __init__(self, channels=None):
        self.channels = channels if channels is not None else []
    def save(self):
        return list(self.channels)