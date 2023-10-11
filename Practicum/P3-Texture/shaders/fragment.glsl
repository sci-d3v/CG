#version 430 core

in vec3 color_f;
in vec2 textureCoordinate_f;

uniform sampler2D texture_f;

out vec4 color;

void main()
{
    color = texture(texture_f, textureCoordinate_f) * vec4(color_f, 1.0f);
    // color = vec4(color_f, 1.0f);
}
//\0