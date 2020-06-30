#!/bin/bash

gcc -c sly.c
gcc -c execute.c
gcc -c main.c
gcc -c searchfile.c
gcc main.o sly.o execute.o searchfile.o -o shellexe.x
