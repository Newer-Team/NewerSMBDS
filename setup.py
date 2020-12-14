import ndspy.rom
import ndspy.code
import zlib
import os
import sys
import shutil
import subprocess


VALID_ROM_CRC = 26695530


if (len(sys.argv) != 2):
    print(F'Usage: {sys.argv[0]} <rom path>')
    sys.exit(1)


rom_path = sys.argv[1]

rom_data = open(rom_path, 'rb').read()
if (zlib.crc32(rom_data) != VALID_ROM_CRC):
    print('ROM is invalid!')
    sys.exit(1)


dst = './original'

def makedir(path):
    try:
        os.mkdir(path)
    except:
        pass


makedir(dst)

def save(file, start, length):
    f = open(F'{dst}/{file}', 'wb')
    f.write(rom_data[start:start+length])
    f.close()

save('arm7.bin', 0x1FE800, 165536)
save('arm7ovt.bin', 0, 0)
save('arm9.bin', 0x4000, 389028)
save('arm9ovt.bin', 0x62FB0, 4192)
save('fnt.bin', 0x226EA0, 41554)
save('header.bin', 0, 512)

makedir(dst + '/overlay7')
makedir(dst + '/overlay9')

rom = ndspy.rom.NintendoDSRom(rom_data)
for id, overlay in rom.loadArm9Overlays().items():
    f = open(F'{dst}/overlay9/overlay9_{id}.bin', 'wb')
    f.write(overlay.save())
    f.close()


files = [
    'arm7.bin',
    'arm9.bin',
    'arm9ovt.bin',
    'header.bin',
]

files.extend([F'overlay9/overlay9_{i}.bin' for i in range(0, 130+1)])


for f in files:
    if not os.path.isfile('original/' + f):
        raise


makedir('Assembly/original/overlay7')
makedir('Assembly/original/overlay9')


def apply_xdelta(original, delta, out):
    subprocess.call(['xdelta3', '-d', '-f', '-s', original, delta, out])


for f in files:
    print(f)

    orig = 'original/' + f
    delta = 'patches/' + f + '.xdelta3'
    out = 'Assembly/original/' + f

    apply_xdelta(orig, delta, out)


open('Assembly/original/arm7ovt.bin', 'wb').close()
shutil.copy('patches/fnt.bin', 'Assembly/original/fnt.bin')

apply_xdelta(rom_path, 'patches/Newer Super Mario Bros. DS.nds.xdelta3', 'Assembly/Newer Super Mario Bros. DS.nds')
print('Newer Super Mario Bros. DS.nds')


shutil.rmtree('./original', ignore_errors=True)
