#version 410 core
layout(location=0)in vec3 position;
layout(location=1)in vec3 color;

uniform mat4 matModel;

out vec3 color_v;

void main(){
    color_v = color;
    // position = normalize(position);
    vec4 coordinate=vec4(position,1)*matModel;
    // vec4 coordinate=vec4(position,1);
    // coordinate = vec4(coordinate.xyz,0.5);
    gl_Position=coordinate;
}