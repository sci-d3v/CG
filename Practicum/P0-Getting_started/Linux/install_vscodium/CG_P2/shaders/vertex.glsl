#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform vec3 translation;
uniform vec3 scale;
uniform float angle;

out vec3 color_f;

void main()
{
    mat4 translationMatrix = mat4(1,vec4(0),1,vec4(0),1,0,translation,1);

    mat4 scaleMatrix=mat4(0);
    scaleMatrix[0][0]=scale.x;
    scaleMatrix[1][1]=scale.y;
    scaleMatrix[2][2]=scale.z;
    scaleMatrix[3][3]=1;

    mat4 rotationMatrix=mat4(cos(angle),sin(angle),vec2(0),-sin(angle),cos(angle),vec4(0),1,vec4(0),1);

    gl_Position = translationMatrix*scaleMatrix*rotationMatrix*vec4(position,1.0);

    color_f = color;
}
//\0