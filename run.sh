#!/bin/bash

g++ src/main.cpp src/glad.c -ldl -lglfw -lGL -lglut -o bin/triangle && bin/triangle
