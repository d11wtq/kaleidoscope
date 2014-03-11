#!/bin/sh
clang++ -O3 main.cpp `llvm-config-3.4 --cppflags --ldflags --libs core jit native`
