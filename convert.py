#!/bin/env python

import puz
import struct
import subprocess
import bitstring
import argparse
import pathlib
from tivars.types import TIAppVar

parser = argparse.ArgumentParser(
    prog='convert.py',
    description='Converts .puz files into CrossBun .8xv puzzle packs')
parser.add_argument('puzzles', metavar='file', type=str, nargs='+',
                    help='a .puz file')
parser.add_argument('output', type=str, help='output appvar file')
parser.add_argument('-t', '--title', required=True, help='pack title')
parser.add_argument('-d', '--description', required=True, help='pack description')
parser.add_argument('-n', '--varname', help='appvar name')

args = parser.parse_args()


def zx0_compress(b):
    cmd = ['convbin', '-i', '/dev/stdin', '-o', '/dev/stdout', '-j', 'bin', '-k', 'bin', '-l', '0', '-c', 'zx0']
    p = subprocess.run(cmd, capture_output=True, input=b)
    if p.returncode != 0: raise RuntimeError('convbin failed')
    return p.stdout


def markup_to_bitmap(m):
    return bitstring.Bits([c == 0x80 for c in m]).tobytes()


magic = b'CROSS'
version = 0

header_format = '<5sBHHB'
entry_format = '<HHHHBBHBHHH'


def read_puzzle(filename):
    with open(filename, 'rb') as f:
        return puz.load(f.read())


puzzles = [read_puzzle(f) for f in args.puzzles]

for p in puzzles:
    if p.width > 15 or p.height > 15:
        print(f'error converting "{p.title}"')
        print('maximum size is 15x15')
        exit(1)
    if p.has_rebus():
        print(f'error converting "{p.title}"')
        print('puzzles with rebus are not supported')
        exit(1)
    if not all(c in '-.' for c in p.fill):
        print(f'error converting "{p.title}"')
        print('puzzles with initial state are not supported')
        exit(1)

pool_start = struct.calcsize(header_format) + struct.calcsize(entry_format) * len(puzzles)
pool = b''
pool_locs = {}


def pool_add(s: bytes | str):
    global pool
    offset = len(pool)
    b = s.encode('ascii') + b'\0' if isinstance(s, str) else s
    if b in pool_locs:
        return pool_locs[b]
    else:
        pool += b
        pool_locs[b] = pool_start + offset
        return pool_start + offset


header = struct.pack(header_format, magic, version, pool_add(args.title), pool_add(args.description), len(puzzles))

entries = [
    struct.pack(
        entry_format,
        pool_add(p.title),
        pool_add(p.notes),
        pool_add(p.author),
        pool_add(p.copyright),
        p.width,
        p.height,
        pool_add(zx0_compress(p.solution.encode('ascii'))),
        len(p.clues),
        len(clue_data),
        pool_add(zx0_compress(clue_data)),
        pool_add(zx0_compress(markup_to_bitmap(p.markup().markup))) if p.has_markup() else 0,
    ) for p in puzzles
    for clue_data in [bytes(b for c in p.clues for b in c.encode('ascii') + b'\0')]
]

if args.varname is None:
    varname = pathlib.Path(args.output).stem.upper()[:8]
else:
    varname = args.varname

assert len(header + b''.join(entries)) == pool_start

data = header + b''.join(entries) + pool

TIAppVar(name=varname, archived=True, data=data).save(args.output)

print(f'wrote {len(puzzles)} puzzles (total {len(data)} bytes) to appvar {varname} in {args.output}')
