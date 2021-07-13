#!/bin/bash


#source ./emsdk_env.sh

#g++ -std=c++11 -o peachtree src/*.cpp  src/options/*.cpp src/aln/*.cpp src/aln/colourings/*.cpp src/phy/*.cpp src/epi/*.cpp src/error/*.cpp
#/opt/cheerp/bin/clang++ -O3 -target cheerp-wasm src/peachtree.cpp -o ../../hello.js

# -s DISABLE_EXCEPTION_CATCHING=0 

emcc -O3 -s WASM=1 -o peachtree.html  -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'UTF8ToString' 'lengthBytesUTF8']" -s "EXPORTED_FUNCTIONS=['_free']"  -std=c++11 -fpermissive -s ALLOW_MEMORY_GROWTH=1 -s WASM_MEM_MAX=1073741824 -s NO_EXIT_RUNTIME=1 src/*.cpp  src/options/*.cpp src/aln/*.cpp src/aln/colourings/*.cpp src/phy/*.cpp src/epi/*.cpp src/wasm/*.cpp 

rm peachtree.html



