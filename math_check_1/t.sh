#!/bin/sh
#compilation and run for testing
#lm is for linking against math library, Wall to see all compiler warnings
gcc math_check.c -o math_check -lm -Wall
./math_check
