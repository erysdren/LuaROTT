#!/bin/bash

# root name
SRC=rotten

# make ico
convert -background transparent -define 'icon:auto-resize=16,24,32,64' ${SRC}.svg ${SRC}.ico

# make pngs
for (( i=16; i<=256; i*=2 ))
do
	inkscape -w $i -h $i ${SRC}.svg -o ${SRC}_${i}x${i}.png
done
