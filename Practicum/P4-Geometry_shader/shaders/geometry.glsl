#version 400 core
layout(lines) in;
layout(line_strip, max_vertices = 256) out;

uniform mat4 matModel;
// recursion steps
uniform int recursionSteps = 0;
// max_count is a maximum number supported by your graphics card
uniform float angleCoefficients[256];

out vec3 color_f;

vec4 rotate_around_z(vec4 v, float angle) {
  float c = cos(angle);
  float s = sin(angle);

  mat4 rotationMatrix = mat4(1.0);
  rotationMatrix[0][0] = c;
  rotationMatrix[0][1] = -s;
  rotationMatrix[1][0] = s;
  rotationMatrix[1][1] = c;

  return rotationMatrix * v;
}

void emit_vertex(vec4 position, vec3 color) {
  gl_Position = position;
  gl_Position *= matModel;
  color_f = color;
  EmitVertex();
}

void main() {
  const int q = 4;
  int count = int(pow(q, recursionSteps));

  if (recursionSteps == 0) {
    // Drawing a simple base case for test
    for (int i = 0; i < gl_in.length(); i++) {
      gl_Position = gl_in[i].gl_Position;
      gl_Position *= matModel;
      color_f = vec3(0, 0, 1);
      EmitVertex();
    }
    EndPrimitive();
  } else {
    vec4 direction = gl_in[1].gl_Position - gl_in[0].gl_Position;
    float direction_length = length(direction);
    direction = normalize(direction);
    direction_length /= int(pow(q-1, recursionSteps));
    float angle = radians(60.0f);

    vec4 next_position = gl_in[0].gl_Position;
    vec3 next_color = vec3(1, 0, 0);
    vec4 next_direction = vec4(0);
    float next_angle = 0.f;

    // Set the first point
    emit_vertex(next_position, next_color);
    // Set another points using algorithm
    for (int ind = 0; ind < count; ind++) {
      next_angle += angleCoefficients[ind] * angle;
      next_direction = rotate_around_z(direction, next_angle);
      next_position += next_direction * direction_length;
      emit_vertex(next_position, next_color);
    }

    EndPrimitive();
  }
}
