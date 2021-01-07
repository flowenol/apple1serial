#!/bin/sh

xa -W -C -v -O ASCII -S -c src/apple1serial.xa -l apple1serial.label -o apple1serial.bin
make -C firmware clean
make -C firmware
