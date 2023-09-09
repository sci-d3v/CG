#version 330 core

in vec3 color_f;
out vec4 color;
void main()
{
    color = vec4(color_f, 1.0f);
}
//\0