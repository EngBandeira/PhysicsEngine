#!/bin/bash
vert=$(glslang src/shaders/vShader.vert)
frag=$(glslang src/shaders/fShader.frag)
geom=$(glslang src/shaders/gShader.geom)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
  ./build.pl   && ./build/final
else
  echo $vert
  echo $frag
  echo $geom
fi
