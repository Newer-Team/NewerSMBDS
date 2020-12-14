
import calendar
import contextlib
import os, os.path


@contextlib.contextmanager
def cwdAs(filepath):
    """
    A context manager that changes the current working directory to the
    directory containing the filepath given. The value bound to the
    target of the `as` clause is the full path to the new directory.
    """
    oldcwd = os.getcwd()
    newcwd = os.path.realpath(filepath)
    os.chdir(newcwd)
    try: yield newcwd
    except:
        # Always reset the CWD, even if there was an exception.
        os.chdir(oldcwd)
        raise
    os.chdir(oldcwd)


def setModifiedTime(filename, when):
    """
    Set the "modified" time of the file described to the time given.
    "when" should be a tuple in the format described by
    https://docs.python.org/3/library/time.html#time.struct_time
    """
    atime = os.stat(filename).st_atime
    mtime = calendar.timegm(when)
    os.utime(filename, times=(atime, mtime))