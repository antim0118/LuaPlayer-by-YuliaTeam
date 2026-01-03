#!/bin/sh
rm -f libs/include_res/output_png.c 
bin2c res/error.png libs/include_res/output_png.c error_data 
make clean && make