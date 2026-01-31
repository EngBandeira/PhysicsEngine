#!/bin/bash
vert=$(glslang src/shaders/shader.vert)
geom=$(glslang src/shaders/shader.geom)
frag=$(glslang src/shaders/texture.frag)
# frag=$(glslang src/shaders/post.frag)

if [[ "$vert" = "" && "$frag" = ""  && "$geom" = ""  ]]; then
    # cd rust && cargo build --release && cd .. && cp rust/target/release/libscripts.a ./build &&
  ./build.pl   && ./build/final
else
  echo $vert
  echo $frag
  echo $geom
fi
