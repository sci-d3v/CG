#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform vec3 translation;
uniform vec3 scale;
uniform float angle;

out vec3 color_f;

void main() {
  mat4 translationMatrix = mat4(1);
  translationMatrix[3] = vec4(translation, 1);

  mat4 scaleMatrix = mat4(1);
  scaleMatrix[0][0] = scale.x;
  scaleMatrix[1][1] = scale.y;
  scaleMatrix[2][2] = scale.z;

  mat4 rotationMatrix = mat4(1);
  rotationMatrix[0] = vec4(cos(angle), sin(angle), 0, 0);
  rotationMatrix[1] = vec4(-sin(angle), cos(angle), 0, 0);

  gl_Position = translationMatrix * rotationMatrix * scaleMatrix * vec4(position, 1.0);

  color_f = color;
}
