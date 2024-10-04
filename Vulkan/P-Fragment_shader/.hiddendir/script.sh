#!/bin/bash

# Set output file name and size
output="output.png"
width=640
height=480

# Write shader code to temporary file
shader=$(mktemp)
cat << GLSL > $shader
precision mediump float;
void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
GLSL

# Render and save image using glslViewer
glslViewer -w $width -h $height -o $output $shader

# Delete temporary shader file
rm $shader
