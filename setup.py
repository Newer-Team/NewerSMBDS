from pathlib import Path
import shutil
import subprocess
import sys
import zlib

import ndspy.code
import ndspy.fnt
import ndspy.rom


VALID_ROM_CRC = 0x197576a
MODULE_PATH = Path(__file__).parent


def apply_xdelta(original: Path, delta: Path, out: Path) -> None:
    """
    Apply an xdelta3 patch
    """
    if sys.platform == 'win32':
        xdelta_path = MODULE_PATH / 'xdelta3'
    else:
        xdelta_path = 'xdelta3'

    subprocess.call([str(xdelta_path), '-d', '-f', '-s', str(original), str(delta), str(out)])


def main():
    if len(sys.argv) != 2:
        print(f'Usage: {sys.argv[0]} <rom path>')
        return


    # Open / check rom

    rom_path = Path(sys.argv[1]).resolve()

    with open(rom_path, 'rb') as f:
        rom_data = f.read()
    if zlib.crc32(rom_data) != VALID_ROM_CRC:
        print('ROM is invalid!')
        return


    # Create a temporary "original" folder

    original_path = MODULE_PATH / 'original'
    original_path.mkdir(exist_ok=True)

    rom = ndspy.rom.NintendoDSRom(rom_data)
    (original_path / 'arm9.bin').write_bytes(rom.arm9)
    (original_path / 'arm9ovt.bin').write_bytes(rom.arm9OverlayTable)
    (original_path / 'arm7.bin').write_bytes(rom.arm7)
    (original_path / 'arm7ovt.bin').write_bytes(rom.arm7OverlayTable)
    (original_path / 'fnt.bin').write_bytes(ndspy.fnt.save(rom.filenames))
    (original_path / 'header.bin').write_bytes(rom_data[:512])

    (original_path / 'overlay9').mkdir(exist_ok=True)
    (original_path / 'overlay7').mkdir(exist_ok=True)

    for id, overlay in rom.loadArm9Overlays().items():
        (original_path / 'overlay9' / f'overlay9_{id}.bin').write_bytes(overlay.data)


    # Apply xdelta patches to things in the "original" folder

    files_to_xdelta_patch = [
        'arm9.bin',
        'arm9ovt.bin',
        'arm7.bin',
        'header.bin',
        *(f'overlay9/overlay9_{i}.bin' for i in range(0, 131)),
    ]

    for f in files_to_xdelta_patch:
        if not (original_path / f).is_file():
            raise RuntimeError(f"File should exist but doesn't: {f}")

    (MODULE_PATH / 'Assembly' / 'original' / 'overlay9').mkdir(exist_ok=True)
    (MODULE_PATH / 'Assembly' / 'original' / 'overlay7').mkdir(exist_ok=True)

    for f in files_to_xdelta_patch:
        print(f)

        orig = original_path / f
        delta = MODULE_PATH / 'patches' / (f + '.xdelta3')
        out = MODULE_PATH / 'Assembly' / 'original' / f

        apply_xdelta(orig, delta, out)


    # A few other things

    (MODULE_PATH / 'Assembly' / 'original' / 'arm7ovt').write_bytes(b'')
    shutil.copy(
        MODULE_PATH / 'patches' / 'fnt.bin',
        MODULE_PATH / 'Assembly' / 'original' / 'fnt.bin')

    apply_xdelta(
        rom_path,
        MODULE_PATH / 'patches' / 'Newer Super Mario Bros. DS.nds.xdelta3',
        MODULE_PATH / 'Assembly' / 'Newer Super Mario Bros. DS.nds')
    print('Newer Super Mario Bros. DS.nds')


    # Delete "original" folder

    shutil.rmtree(original_path, ignore_errors=True)


main()
