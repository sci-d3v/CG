#version 410 core
layout(location=0)in vec3 position;

uniform mat4 matModel;

void main(){
    vec4 coordinate=vec4(position,1)*matModel;
    gl_Position=coordinate;
}
//\0
