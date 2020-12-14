import enum


class Processor(enum.Enum):
    """
    An enum that enumerates the Nintendo DS's two processors.
    """
    ARM9 = 9
    ARM7 = 7


def indexInNamedList(L, name):
    """
    For a list in the format
    [(name1, entry1), (name2, entry2), (name3, entry3), ...]
    Return the index of the entry corresponding with name, or raise
    KeyError if it doesn't exist.
    """
    for i, (nameN, entry) in enumerate(L):
        if nameN == name:
            return i
    raise KeyError(f'{name} not found in the list...')


def findInNamedList(L, name):
    """
    For a list in the format
    [(name1, entry1), (name2, entry2), (name3, entry3), ...]
    Return the entry corresponding with name, or raise KeyError if it
    doesn't exist.
    """
    return L[indexInNamedList(L, name)][1]


def setInNamedList(L, name, value):
    """
    For a list in the format
    [(name1, entry1), (name2, entry2), (name3, entry3), ...]
    Replace the entry corresponding with name, with value. Raise
    KeyError if it doesn't exist.
    """
    # Can't assign to tuples, so we need to make a new one
    L[indexInNamedList(L, name)] = (name, value)