# NEWER Super Mario Bros. DS
## Source Code Public Release

### Preamble

We really need to stop putting off releasing the source code
in hopes of cleaning it up. Never works. It's a trap.

But, well, here it is. This is all the custom code that made
it into the game, plus some extras that did not.

Please excuse the formatting of the code; we haven't had time to fix it properly yet.

### Licensing

The Newer DS source code and tools are released under the GPLv3 license.
See the `LICENSE` file in this repository for full details.

Copyright (c) 2015-2017 Skawo, RoadrunnerWMC, RicBent

### Requirements

Please note: you must use *exactly* the versions of devkitARM and libnds described below. **Newer versions will not work.**

- Python 3
- ndspy
- devkitARM version 47 (available here: https://web.archive.org/web/20241207013909/https://wii.leseratte10.de/devkitPro/devkitARM/r47%20(2017))
- libnds version 1.7.2 (available here: https://web.archive.org/web/20240409062540/https://wii.leseratte10.de/devkitPro/libnds/libnds_1.7.2%20%282018%29/)
- A North American New Super Mario Bros. DS rom (md5: a2ddba012e5c3c2096d0be57cc273be5)

## Installation

### Windows

First install a version of Python 3. Next run:

``py -3 -m pip install ndspy``

Download libnds-1.7.2.tar.bz2 and devkitARM_r47-win32.exe from the directories linked above and extract them (The devkitARM .exe does the extracting by itself). Next, run the following commands:

``SETX LIBNDS172 "[[Extracted LIBNDS Path]]"``

``SETX DEVKITARM47 "[[Extracted devkitARM Path]]"``

Then close your Command Prompt window and re-open it, which allows the changes to take effect.


### Usage

To start, run the following command:

``py -3 setup.py "[[NSMB ROM Path]]"``

This will set up the "Assembly" folder. After this, simply run "build.bat" to compile the code.

## Troubleshooting

### Compilation failed, and above that it says "`cannot find -lgcc`"

You probably installed the wrong version of devkitARM. Make sure you're using *exactly* the version described in the "Requirements" section above.

(Technical explanation: Magigoomba's `Makefile` requires a specific version of gcc, but newer devkitARM releases come with different versions of it. The mismatch prevents the libgcc headers from being found.)
