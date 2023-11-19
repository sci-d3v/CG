#version 330 core
layout(location=0)in vec3 position;
layout(location=1)in vec3 color;

out VertexData{
    vec3 color;
}extra_data;

uniform mat4 matModel;

void main(){
    gl_Position=vec4(position,1)*matModel;
    extra_data.color=color;
}
//\0