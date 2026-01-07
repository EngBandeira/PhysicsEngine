#!/bin/bash
vert=$(glslang vShader.vert)
frag=$(glslang fShader.frag)
geom=$(glslang gShader.geom)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
  ./build.pl   && ./build/final
else
  echo $vert
  echo $frag
  echo $geom
fi
/home/bandeira/Documents/GIT/PhysicsEngine/../../
