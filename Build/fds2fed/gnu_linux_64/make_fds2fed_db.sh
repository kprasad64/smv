#!/bin/bash
rm -f *.o *.mod
make -f ../Makefile gnu_linux_64_db -j 4
