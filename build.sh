#!/bin/bash
vert=$(glslang src/shaders/shader.vert)
frag=$(glslang src/shaders/shader.frag)
geom=$(glslang src/shaders/shader.geom)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
  ./build.pl   && ./build/final
else
  echo $vert
  echo $frag
  echo $geom
fi
