#!/bin/bash

if [ ! -d build ]; then
  mkdir -p build;
fi

g++ -g -o build/openglgame.exe GLM/glm.cpp GLM/glmimg.cpp GLM/stdafx.cpp GLM/Texture.cpp main.cpp -lglut32 -lopengl32 -lglu32 #-Lglut

echo "BUILT"

cd build
./openglgame

exit 0
