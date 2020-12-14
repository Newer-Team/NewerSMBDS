import argparse

import _common

import ndspy.fnt


def main():
    """
    Main function for FNT Tool.
    """

    parser = argparse.ArgumentParser(
        description='FNT Tool: Convert fnt.bin files to and from JSON.')

    parser.add_argument('infile',
                        help='the file to be converted')
    parser.add_argument('outfile',
                        nargs='?', # https://stackoverflow.com/a/4480202
                        help='the output filename')

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-j', '--json',
                       action='store_true',
                       help='convert BIN to JSON'
                            " (don't try to guess the input filetype)")
    group.add_argument('-b', '--bin',
                       action='store_true',
                       help='convert JSON to BIN'
                            " (don't try to guess the input filetype)")

    args = parser.parse_args()

    with open(args.infile, 'rb') as f:
        ind = f.read()

    if args.json: # Convert BIN -> JSON
        isBinary = True
    elif args.bin: # Convert JSON -> BIN
        isBinary = False
    else:
        # Is this a JSON (text) or BIN (binary) file? Let's use the simplest
        # heuristic imaginable:
        isBinary = b'\0' in ind
        # Seems fragile, but really, text files will never include a null
        # byte (unless there's a null character, which is in fact valid
        # UTF-8, but I choose not to worry about that edge case), and
        # fnt.bin files include the value 0xF000 near the beginning. So this
        # should work every time unless there's a null character in the JSON
        # (in which case, why do you have null characters in your JSON?)

    outfile = args.outfile
    if outfile is None:
        outfile = args.infile + ('.json' if isBinary else '.bin')

    convType = 'BIN -> JSON' if isBinary else 'JSON -> BIN'
    print(f'Converting {args.infile} to {outfile} ({convType})...')

    if isBinary:
        outd = _common.fntToJson(ndspy.fnt.load(ind))
        with open(outfile, 'w', encoding='utf-8') as f:
            f.write(outd)
    else:
        outd = ndspy.fnt.save(_common.jsonToFnt(ind.decode('utf-8')))
        with open(outfile, 'wb') as f:
            f.write(outd)

    print('Done.')


if __name__ == '__main__':
    main()
