#version 410 core

in vec3 color_g;

void main(){
    gl_FragColor= vec4(color_g,1);
}