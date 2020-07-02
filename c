#!/bin/bash

gcc -c sly.c
gcc -c execute.c
gcc -c main.c -lreadline
gcc -c searchfile.c
gcc main.o sly.o execute.o searchfile.o -o shellexe.x -lreadline
rm sly.o
rm execute.o
rm main.o
rm searchfile.o
rm ~/.history