//Use Ctrl+Shift+P and write a command "Show glslCanvas" for live preview a fragment shaders (VSCodium)

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
#define PI 3.1415926

/*
    Функция, которая проверяет находятся ли точки pos и p0
    по одну сторону полуплоскости, получаемой в результате отсечения
    прямой, проходящей через точки v0 и v1.
*/
bool points_line_side(vec2 pos, vec2 p0, vec2 v0, vec2 v1) {
  return (pos.x - v0.x) * (v1.y - v0.y) < (pos.y - v0.y) * (v1.x - v0.x) &&
         (p0.x - v0.x) * (v1.y - v0.y) < (p0.y - v0.y) * (v1.x - v0.x);
}

/*
    Функция, которая проверяет находятся ли точка pos в области,
    которая получается с помощью двух пересекающихся линий.
*/
bool point_outside(vec2 pos, vec2 p0, vec2 v0, vec2 v1, vec2 v2) {
  if (!points_line_side(pos, p0, v0, v1) && !points_line_side(pos, p0, v1, v2))
    return true;
  return false;
}

/*
    Функция, которая рисует звезду
    pos - позиция точки, для которой задаем цвет;
    p0 - местоположение центра звезды;
    r - внутренний радиус (внутренняя граница звезды);
    R - внешний радиус (внешняя граница звезды);
    angle - угол поворота;
    color - цвет звезды;
    background_color - цвет фона.
*/
vec4 Star(vec2 pos, vec2 p0, float r, float R, float angle, vec4 color,
          vec4 background_color) {
  // Минимальное количество внешних вершин у звезды может быть задано 3
  const int count = 5;
  vec4 color_ = background_color;
  // Внешняя граница звезды определяется уравнением окружности
  if (pow(pos.x - p0.x, 2.0) + pow(pos.y - p0.y, 2.0) < pow(R, 2.0)) {
    // Устанавливаем цвет звезды
    color_ = color;
    for (int i = 0; i < count; ++i) {
      // 3 вершины образуют отсекаемую часть области, из них
      // две внешние и одна внутренняя
      vec2 vertices[3];
      for (int j = 0; j < 3; ++j) {
        // Итоговый порядковый номер вершины с учетом внутренних
        int index = i * 2 + j;
        // Угол, на который необходимо повернуть вершину
        // в зависимости от числа вершин, порядкового номера и общего угла
        float angle_ = angle + float(index) * PI / float(count);
        // Позиция смещения, относительно центра звезды
        vec2 shift =
            vec2(mod(float(index), 2.0) * r + mod(float(index + 1), 2.0) * R)
                .xy *
            vec2(cos(angle_), sin(angle_)).xy;
        // Позиция с учетом всего двумерного пространства
        vertices[j] = p0 + shift;
      }
      // Если точка принадлежит отсекаемой части области,
      // то отбрасываем цвет
      if (point_outside(pos, p0, vertices[0], vertices[1], vertices[2]))
        color_ = background_color;
    }
  }
  return color_;
}

void main() {
  vec2 position = gl_FragCoord.xy / u_resolution.xy;
  gl_FragColor = Star(position, vec2(0.5), 0.2, 0.5, 0.25 * PI,
                      vec4(1, 0, 0, 1), vec4(0, 0, 1, 1));
}
