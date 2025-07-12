#!/bin/bash
vert=$(glslang vShader.vert)
frag=$(glslang fShader.frag)
geom=$(glslang gShader.geom)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
  lua Make.lua && ./build/final
else 
  echo $vert
  echo $frag
  echo $geom
fi