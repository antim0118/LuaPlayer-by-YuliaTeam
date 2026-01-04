#!/bin/sh
# convert error pic
rm -f libs/include_res/output_png.c 
bin2c res/error.png libs/include_res/output_png.c error_data 
# convert error sound
rm -f libs/include_res/output_wav.c 
bin2c res/error.wav libs/include_res/output_wav.c system_sound 
# convert default font
rm -f libs/include_res/output_pgf.c 
bin2c res/luaFont.pgf libs/include_res/output_pgf.c LUAFont 

# build
make clean && make