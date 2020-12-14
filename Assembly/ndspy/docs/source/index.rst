.. ndspy documentation master file.

ndspy
=====

**ndspy** is primarily a Python library that provides tools for reading, modifying and creating many common file formats used in Nintendo DS games. The APIs are designed to be low-level to allow for maximum control and flexibility. Accuracy is a key goal -- whenever possible, loading and saving a file used in an official game should result in the original file data, byte-for-byte, even for highly complex file formats.

ndspy also includes a set of simple tools that make use of the library. These let you use the library to convert between file formats without having to write any code of your own.

If you plan to use ndspy for sound data and aren't familiar with *SDAT* files, consider reading :doc:`the page explaining how they're structured <sdat_structure>` first.

.. toctree::
    :maxdepth: 2
    :caption: Contents

    bmg
    sdat_structure

Credits
-------

**ndspy** was written by `RoadrunnerWMC <https://github.com/RoadrunnerWMC/>`_, using information from many, many sources. In alphabetical order:

* `Custom Mario Kart Wiiki <http://wiki.tockdom.com/>`_ -- for information on the version of *BMG* files used in Wii games (which isn't the same as the version of *BMG* used in DS games, but is similar)
* `DS Sound Studio <http://archive.dshack.org/thread.php?tid=2590>`_ -- for the meaning of sequence players and stream players
* `Source code for DSDecmp <https://github.com/Barubary/dsdecmp>`_ (`homepage <http://www.romhacking.net/utilities/789/>`_) -- for code for several compression formats
* `DSiBrew <http://www.dsibrew.org/>`_ -- for some more info about the ROM file format
* `Imran Nazar: The Smallest NDS File <http://imrannazar.com/The-Smallest-NDS-File>`_ -- convenient quick reference for the ROM header format; also provided a nice test case for the ROM library code
* `kiwi.ds Nitro Composer File (*.sdat) Specification <https://sites.google.com/site/kiwids/sdat.html>`_ -- probably the best overall reference for *SDAT*
* `Nintendo DS File Formats <http://www.romhacking.net/documents/[469]nds_formats.htm>`_ -- a terrific reference for a wide variety of format specifications
* `Source code for Nintendo DS/GBA Compressors by CUE <http://www.romhacking.net/utilities/826/>`_ (`thread <https://gbatemp.net/threads/nintendo-ds-gba-compressors.313278/>`_) -- for code for reversed LZ compression (code compression)
* `Source code for NSMB Editor (NSMBe) <https://github.com/Dirbaio/NSMB-Editor>`_ -- for information and code for many formats
* `Source code for sseq2mid <https://github.com/loveemu/loveemu-lab/tree/master/sseq2mid/src2>`_ -- supports more types of sequence events than are documented in other references
* `Source code for Tinke <https://github.com/pleonex/tinke>`_ -- fills in the gaps in *SDAT* where the other references are ambiguous
* Some original research by me and `skawo <https://www.youtube.com/user/skawo90>`_.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
