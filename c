#!/bin/bash

gcc -c sly.c
gcc -c execute.c
gcc -c main.c -lpthread
gcc -c searchfile.c
gcc main.o sly.o execute.o searchfile.o -o shellexe.x -lpthread
rm sly.o
rm execute.o
rm main.o
rm searchfile.o
