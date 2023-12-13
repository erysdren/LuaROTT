#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import struct

class Lump:
	def __init__(self):
		self.ofs = 0
		self.len = 0
		self.name = ""
		self.data = bytes()

	def set_name(self, name):
		self.name = name.ljust(8, '\0')[:8].encode("ascii")

	def load_data(self, filename):
		fh = open(filename, "rb")
		self.data = bytes(fh.read())
		self.len = len(self.data)
		self.set_name(filename)
		fh.close()

	def write_data(self, fh):
		self.ofs = fh.tell()
		fh.write(self.data)

	def write_lump(self, fh):
		fh.write(struct.pack("<I", self.ofs))
		fh.write(struct.pack("<I", self.len))
		fh.write(bytes(self.name))

if __name__ == "__main__":

	# open file
	wadfile = open("rotten.wad", "wb")

	# header
	wadfile.write(bytes("IWAD".encode("ascii")))
	wadfile.write(struct.pack("<I", 0))
	wadfile.write(struct.pack("<I", 0))

	# lumps array
	lumps = []

	# start marker
	dopestrt = Lump()
	dopestrt.set_name("DOPESTRT")
	dopestrt.ofs = wadfile.tell()
	dopestrt.len = 0
	lumps.append(dopestrt)

	# get lump data
	for i in range(8):
		filename = "DOPE" + str(i + 1)
		lump = Lump()
		lump.load_data(filename)
		lumps.append(lump)

	# write lump data
	for lump in lumps:
		lump.write_data(wadfile)

	# end marker
	dopestop = Lump()
	dopestop.set_name("DOPESTOP")
	dopestop.ofs = wadfile.tell()
	dopestop.len = 0
	lumps.append(dopestop)

	# write header
	num_lumps = len(lumps)
	ofs_lumps = wadfile.tell()
	wadfile.seek(4)
	wadfile.write(struct.pack("<I", num_lumps))
	wadfile.write(struct.pack("<I", ofs_lumps))
	wadfile.seek(ofs_lumps)

	# write lump array
	for lump in lumps:
		lump.write_lump(wadfile)

	# close file
	wadfile.close()
