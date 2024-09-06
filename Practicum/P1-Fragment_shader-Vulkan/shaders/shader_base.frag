#version 460

layout(binding = 0) uniform UniformBufferObject {
  vec2 u_resolution;
};

layout(location = 0) out vec4 outColor;

vec4 RussianFlag(vec2 position) {
  vec4 color = vec4(0);
  if (position.y <= 0.22)
    color = vec4(1, vec2(0), 1);
  else if (position.y <= 0.44)
    color = vec4(vec2(0), vec2(1));
  else if (position.y <= 0.66)
    color = vec4(1);
  else
    color = vec4(0, 0, 0, 0);

  return color;
}

void main() {
  vec2 position = gl_FragCoord.xy / u_resolution.xy;
  position.y = -1.0 * position.y + 1.0;
  outColor = RussianFlag(position);
}
