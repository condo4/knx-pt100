#!/bin/sh
echo $*
echo "FLASH SCRIPT"
st-flash read backup.img 0x0803F800 0x800
st-flash erase
st-flash write backup.img 0x0803F800
st-flash write $1 0x8000000
rm backup.img
