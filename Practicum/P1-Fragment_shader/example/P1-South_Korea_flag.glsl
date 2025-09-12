// Use Ctrl+Shift+P and write a command "Show glslCanvas" for live preview a fragment shaders (VSCodium)

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;

const float PI = 3.1415926;

float sqr(float value) {
  return value * value;
}

vec2 sqr2(vec2 value) {
  return vec2(value.x * value.x, value.y * value.y);
}

// Rotate a 2D position by given angle (in radians)
vec2 Rotate(vec2 position, float angle) {
  mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
  vec2 new_position = rotation * position;
  return new_position;
}

// Check if point is on one side of a line defined by two points
// Uses parametric line equation: (x - x_0)/(x_1-x_0) <= (y-y_0)/(y_1-y_0)
bool Line(vec2 position, vec2 p0, vec2 p1) {
  vec2 re = (position - p0) / (p1 - p0);
  return re.x <= re.y;
}

// Check if point is inside a rectangle defined by center and size
bool Rectangle(vec2 position, vec2 center, vec2 size) {
  vec2 position_ = sqr2(position - center);
  vec2 size_ = sqr2(size);
  return position_.x <= size_.x && position_.y <= size_.y;
}

// Check if point is inside a circle defined by center and radius
bool Circle(vec2 position, vec2 center, float radius) {
  vec2 position_ = sqr2(position - center);
  return position_.x + position_.y <= sqr(radius);
}

// Main function to render the South Korea flag
vec4 SouthKoreaFlag(vec2 position) {
  // Define colors used in the flag
  // https://www.flagcolorcodes.com/south-korea
  vec4 transparentColor = vec4(0.0, 0.0, 0.0, 0.0);
  vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 yinColor = vec4(15.0, 100.0, 205.0, 255.0) / 255.0;
  vec4 yangColor = vec4(205.0, 46.0, 58.0, 255.0) / 255.0;
  vec4 trigramColor = vec4(0.0, 0.0, 0.0, 1.0);

  // The scheme was taken from
  // https://www.wikiwand.com/en/articles/Flag_of_South_Korea#/media/File:Flag_of_South_Korea_(construction_sheet).svg

  // Flag dimensions (aspect ratio 3:2)
  vec2 sizeFlag = vec2(72.0, 48.0);
  float maxSizeFlag = max(sizeFlag.x, sizeFlag.y);
  // Normalize flag size
  sizeFlag /= maxSizeFlag;
  vec2 centerFlag = vec2(0.5) * sizeFlag;

  // Check flag bounds (border)
  if (Line(position, sizeFlag + vec2(0.1, 0.0), sizeFlag)) {
    return transparentColor;
  }
  
  // Draw the central Taegeuk (yin-yang symbol)
  // Main yin-yang circle
  if (Circle(position, centerFlag, 24.0 * 0.5 / maxSizeFlag)) {
    // small radius for the small circles
    float smallRadius = 24.0 * 0.5 / maxSizeFlag * 0.5;

    // Small blue circle (upper part of yin-yang)
    if (Circle(position, centerFlag + Rotate(vec2(smallRadius, 0.0), atan(2.0, 3.0)), smallRadius)) {
      return yinColor;
    }

    // Small red circle (lower part of yin-yang)
    if (Circle(position, centerFlag - Rotate(vec2(smallRadius, 0.0), atan(2.0, 3.0)), smallRadius)) {
      return yangColor;
    }

    // Divide the circle into red and blue halves
    if (Line(position, centerFlag, vec2(sizeFlag.x, 0.0))) {
      return yinColor;
    } else {
      return yangColor;
    }
  }
  // Draw the four trigrams (corner symbols)
  {
    vec2 rectangleSize = vec2(4.0, 6.0) / maxSizeFlag;
    // 24 / 2 + 6 + 6
    vec2 rectangleShift = vec2(24.0, 0.0) / maxSizeFlag;
    float angle;

    // Top-left trigram (Geon - ☰)
    angle = -atan(2.0, 3.0) + PI;
    if (Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, rectangleSize) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift - vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag)) {
      return trigramColor;
    }
    // Top-right trigram (Gam - ☵)
    angle = atan(2.0, 3.0);
    if (Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, rectangleSize) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift - vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(3.0, 0.0) / maxSizeFlag, vec2(1.0, 0.5) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(-3.0, 0.0) / maxSizeFlag, vec2(1.0, 0.5) / maxSizeFlag)) {
      return trigramColor;
    }
    // Bottom-left trigram (Ri - ☲)
    angle = atan(2.0, 3.0) + PI;
    if (Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, rectangleSize) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift - vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, vec2(1.0, 0.5) / maxSizeFlag)) {
      return trigramColor;
    }

    // Bottom-right trigram (Gon - ☷)
    angle = -atan(2.0, 3.0);
    if (Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, rectangleSize) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift + vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift - vec2(1.5 / maxSizeFlag, 0.0), vec2(0.5, 6.0) / maxSizeFlag) 
        &&
        !Rectangle(Rotate(position, angle), Rotate(centerFlag, angle) + rectangleShift, vec2(4.0, 0.5) / maxSizeFlag)) {
      return trigramColor;
    }
  }

  return backgroundColor;
}

// Main shader entry point
void main() {
  // Convert fragment coordinates to normalized position [0,1]
  vec2 position = gl_FragCoord.xy / u_resolution.xy;

  // Render the South Korea flag
  gl_FragColor = SouthKoreaFlag(position);
}
