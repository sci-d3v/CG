#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageTexture;

void main() {
  vec4 color = texture(imageTexture, TexCoords);
  FragColor = vec4(color);
}
