# dts
Simple Ditital Signage

Compile:

gcc -g -Wall -o dts-simple dts-simple.c dts_functions.c  `pkg-config --libs --cflags gtk+-2.0` -L/usr/lib/aarch64-linux-gnu/ -lmariadb
