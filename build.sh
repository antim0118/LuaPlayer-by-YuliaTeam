#!/bin/sh
rm -f libs/include_res/output_png.c 
bin2c res/error.png libs/include_res/output_png.c error_data 
rm -f libs/include_res/output_wav.c 
bin2c res/error.wav libs/include_res/output_wav.c system_sound 
make clean && make