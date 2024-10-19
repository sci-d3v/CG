#version 400 core
layout(lines) in;
layout(line_strip, max_vertices = 99) out;

uniform mat4 matModel;
// recursion steps
uniform int recursion_steps = 0;

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
  // see test 0
  // on the first step must be 4 vertices;
  // on the second step must be 16 vertices;
  // etc.
  int q = 4;
  int count = int(pow(q, recursion_steps));
  // max_count is a maximum number supported by your graphics card
  const int max_count = 50;
  // Vertex array
  float angle_coefficients[max_count];
  // Only for the first steps
  angle_coefficients[0] = 1.f;
  angle_coefficients[1] = -2.f;
  angle_coefficients[2] = 1.f;

  if (recursion_steps == 0) {
    // Drawing a simple base case for test
    for (int i = 0; i < gl_in.length(); i++) {
      gl_Position = gl_in[i].gl_Position;
      color_f = vec3(0,0,1);
      EmitVertex();
    }
    EndPrimitive();
  } else {
    vec4 direction = gl_in[1].gl_Position - gl_in[0].gl_Position;
    float direction_length = length(direction);
    direction = normalize(direction);
    direction_length /= 4.0;
    float angle = radians(60.0f);

    vec4 next_position = gl_in[0].gl_Position;
    vec3 next_color = vec3(1, 0, 0);
    vec4 next_direction = vec4(0);

    // Draw the first line
    emit_vertex(next_position, next_color);
    next_direction = normalize(rotate_around_z(direction, 0.f * angle));
    next_position += next_direction * direction_length;
    emit_vertex(next_position, next_color);

    // Draw another lines
    for (int ind = 0; ind < q; ind++) {
      next_direction = normalize(
          rotate_around_z(next_direction, angle_coefficients[ind] * angle));
      next_position += next_direction * direction_length;
      emit_vertex(next_position, next_color);
    }

    EndPrimitive();
  }
}
