# MIT License
#
# Copyright (c) 2017 RoadrunnerWMC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import collections
import struct


class Folder:
    """
    A class that represents a folder. A folder can contain other folders
    (.folders) and files (.files). A folder also has a "firstID", which
    is the ID of the first file in it. (The rest of the files are
    assigned firstID + 1, firstID + 2, ...)
    """
    def __init__(self, folders=None, files=None, firstID=None):
        if folders is not None:
            self.folders = folders
        else:
            self.folders = collections.OrderedDict()
        if files is not None:
            self.files = files
        else:
            self.files = []
        self.firstID = firstID


    def __iter__(self):
        raise ValueError('Sorry, a Folder is not iterable.')


    def __getitem__(self, key):
        """
        Convenience function:
        - for an integer key, calls filenameOf()
        - for a string key:
            - calls idOf() if key refers to a file, or
            - calls subfolder() if key refers to a directory.
        """
        if isinstance(key, int):
            return self.filenameOf(key)
        elif isinstance(key, str):
            fileID = self.idOf(key)
            if fileID is None:
                return self.subfolder(key)
            return fileID
        else:
            raise TypeError('Folders can only convert between strings'
                            f' and ints, not "{type(key)}".')


    def __contains__(self, key):
        return self.__getitem__(key) is not None


    def idOf(self, path):
        """
        Get the file ID for the given filename or slash-separated
        file path relative to this folder.
        Return None if no such file exists.
        """

        def findInFolder(requestedPath, searchFolder):
            """
            Attempt to find filename in the given folder.
            pathSoFar is the path up through this point, as a list.
            """
            pathPart = requestedPath[0]
            if len(requestedPath) == 1:
                # It's hopefully a file in this folder.
                if pathPart in searchFolder.files:
                    # Yay!
                    return searchFolder.firstID + searchFolder.files.index(pathPart)
                else:
                    # Not here.
                    return None

            # Hopefully we have the requested subfolder...
            if pathPart in searchFolder.folders:
                # Yup.
                return findInFolder(requestedPath[1:], searchFolder.folders[pathPart])
            # Welp.
            return None

        pathList = path.split('/')
        while not pathList[-1]: pathList = pathList[:-1]
        while not pathList[0]: pathList = pathList[1:]
        return findInFolder(pathList, self)


    def subfolder(self, path):
        """
        Similar to idOf(), except it searches for subfolders and returns
        None or the appropriate Folder instance.
        """

        def findInFolder(requestedPath, searchFolder):
            """
            Attempt to find filename in the given folder.
            pathSoFar is the path up through this point, as a list.
            """
            pathPart = requestedPath[0]
            if len(requestedPath) == 1:
                # It's hopefully a subfolder in this folder.
                if pathPart in searchFolder.folders:
                    # Yay!
                    return searchFolder.folders[pathPart]
                else:
                    # Not here.
                    return None

            # Hopefully we have the requested subfolder...
            if pathPart in searchFolder.folders:
                # Yup.
                return findInFolder(requestedPath[1:], searchFolder.folders[pathPart])
            # Welp.
            return None

        pathList = path.split('/')
        while not pathList[-1]: pathList = pathList[:-1]
        while not pathList[0]: pathList = pathList[1:]
        return findInFolder(pathList, self)


    def filenameOf(self, id):
        """
        Get the filename of a given file ID.
        Return None if no such file exists, or if the file exists but is
        unnamed.
        """

        def findInFolder(pathSoFar, searchFolder):
            """
            Attempt to find id in the given folder.
            pathSoFar is the path up through this point, as a list.
            """
            # Check if it's in this folder
            firstID = searchFolder.firstID
            if firstID <= id < firstID + len(searchFolder.files):
                # Found it!
                filename = searchFolder.files[id - firstID]
                return [*pathSoFar, filename]

            # Check subfolders
            for subfolderName, subfolder in searchFolder.folders.items():
                result = findInFolder([*pathSoFar, subfolderName], subfolder)
                if result is not None:
                    # Found it in that folder!
                    return result

            # Didn't find it.
            return None

        result = findInFolder([], self)
        if result is not None:
            return '/'.join(result)
        else:
            return None


    def _strList(self, indent=0):
        L = []
        indentStr = ' ' * indent

        for folderName, folder in self.folders.items():
            L.append(indentStr + folderName + '/')
            L.extend(folder._strList(indent + 4))

        for fileName in self.files:
            L.append(indentStr + fileName)

        return L

    def __str__(self):
        return '\n'.join(self._strList(0))


def load(fnt):
    """
    Convert a filenames table to a Folder instance.
    """
    def loadFolder(folderId):
        """
        Load the folder with ID `folderId` and return it as a dict.
        """
        folderObj = Folder()

        # Get the entries table offset and file ID from the top of the
        # fnt file
        off = 8 * (folderId & 0xFFF)
        entriesTableOff, fileID = struct.unpack_from('<IH', fnt, off)
        folderObj.firstID = fileID

        off = entriesTableOff

        # Read file and folder entries from the entries table
        while True:
            control, = struct.unpack_from('B', fnt, off); off += 1
            if control == 0:
                break

            # That first byte is a control byte that includes the length
            # of the upcoming string and if this entry is a folder
            len_, isFolder = control & 0x7F, control & 0x80

            name = fnt[off : off+len_].decode('latin-1'); off += len_

            if isFolder:
                # There's an additional 2-byte value with the subfolder
                # ID. Get that and load the folder
                subFolderID, = struct.unpack_from('<H', fnt, off); off += 2
                folderObj.folders[name] = loadFolder(subFolderID)
            else:
                folderObj.files.append(name)

        return folderObj

    # Root folder is always 0xF000
    return loadFolder(0xF000)


def save(root):
    """
    Convert a Folder instance back to a filenames table.
    """

    # folderEntries is a dict of tuples:
    # {
    #     folderID: (initialFileID, parentFolderID, b'file entries data'),
    #     folderID: (initialFileID, parentFolderID, b'file entries data'),
    # }
    # This is an intermediate representation of the filenames data that
    # can be converted to the final binary representation much more
    # easily than the nested dicts can.
    folderEntries = {}

    # nextFolderID allows us to assign folder IDs in sequential order.
    # The root folder always has ID 0xF000.
    nextFolderID = 0xF000

    def parseFolder(d, parentID):
        """
        Parse a folder dictionary and add its entries to folderEntries.
        `parentID` is the ID of the folder containing this one.
        """

        # Grab the next folder ID
        nonlocal nextFolderID
        folderID = nextFolderID
        nextFolderID += 1

        # Create an entries table and add filenames and folders to it
        entriesTable = bytearray()
        for file in d.files:
            # Each file entry is preceded by a 1-byte length value.
            entriesTable.extend(bytes([len(file)]))
            entriesTable.extend(file.encode('latin-1'))

        for folderName, folder in d.folders.items():
            # First, parse the subfolder dict and get its ID, so we can
            # save that to the entries table.
            otherID = parseFolder(folder, folderID)

            # Folder name is preceded by a 1-byte length value, OR'ed
            # with 0x80 to mark it as a folder.
            entriesTable.extend(bytes([len(folderName) | 0x80]))
            entriesTable.extend(folderName.encode('latin-1'))

            # And the ID of the subfolder goes after its name, as a
            # 2-byte value.
            entriesTable.extend(struct.pack('<H', otherID))

        # And the entries table needs to end with a null byte to mark
        # its end.
        entriesTable.extend(b'\0')

        folderEntries[folderID] = (d.firstID, parentID, entriesTable)
        return folderID

    # The root folder ID is the total number of folders.
    def countFoldersIn(folder):
        folderCount = 0
        for _, f in folder.folders.items():
            folderCount += countFoldersIn(f)
        return folderCount + 1
    rootId = countFoldersIn(root)

    # Ensure that the root folder has the proper folder ID.
    assert parseFolder(root, rootId) == 0xF000

    # Allocate space for the folders table at the beginning of the file
    fnt = bytearray(len(folderEntries) * 8)

    # We need to iterate over the folders in order of increasing ID.
    for currentFolderID in sorted(folderEntries.keys()):
        fileID, parentID, entriesTable = folderEntries[currentFolderID]

        # Add the folder entries to the folder table
        offsetInFolderTable = 8 * (currentFolderID & 0xFFF)
        struct.pack_into('<IHH', fnt, offsetInFolderTable,
            len(fnt), fileID, parentID)

        # And tack the folder's entries table onto the end of the file
        fnt.extend(entriesTable)

    return fnt
