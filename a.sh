#!/bin/bash
vert=$(glslang vShaders.vert)
frag=$(glslang fShaders.frag)
geom=$(glslang gShaders.geom)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
  rm build/*
  make
  ./build/final
else 
  echo $vert
  echo $frag
  echo $geom
fi