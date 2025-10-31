#version 430 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imageTexture;

uniform vec2 texture_size;

void main() {
  if (gl_GlobalInvocationID.x > texture_size.x ||
      gl_GlobalInvocationID.y > texture_size.y) {
    return;
  }

  // Текущая координата
  ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

  ivec2 center = ivec2(texture_size) / 2;
  int distance = abs(texelCoord.x - center.x) + abs(texelCoord.y - center.y);

  // Оттенок в зависимости от расстояния
  float hue = mod(float(distance) / 10.0, 1.0);

  // Преобразуем оттенок в RGB
  vec4 totalColor = vec4(cos(6.28318 * hue), cos(6.28318 * (hue - 0.33)),
                         cos(6.28318 * (hue - 0.66)), 1.0);
  imageStore(imageTexture, texelCoord, totalColor);
}
