#!/bin/sh
gcc -DGL_GLEXT_PROTOTYPES -O3 -o view -iquote include src/*.c -lGL -lGLU -lglut -lm
