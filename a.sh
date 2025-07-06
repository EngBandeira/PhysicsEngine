#!/bin/bash
vert=$(glslang vShaders.vert)
frag=$(glslang fShaders.frag)

if [[ "$vert" = "" && "$frag" = ""  ]]; then
  rm build/*
  make
  ./build/final
else 
  echo $vert
  echo $frag
fi