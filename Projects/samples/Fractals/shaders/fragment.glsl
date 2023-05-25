uniform vec2 u_resolution;
uniform vec2 u_shift;
uniform vec2 u_scale;

uniform int u_fractal_iteration;
uniform int u_fractal_type;

/*
    Функция, которая рисует множество Мандельброта
    position - позиция точки экрана, для которой устанавливается цвет;
    shift - смещение позиции относительно центра после масштабирования;
    scale - масштаб.
*/
vec4 MandelbrotSet(vec2 position, vec2 shift, vec2 scale)
{
    // Новая позиция с учетом масштабирования и смещения
    vec2 position_ = (position-vec2(0.5))*scale - shift;
    // Для множества Мандельброта z0=0
    vec2 zn = vec2(0);

    // Количество итераций в цикле влияет на четкость изображения
    // const int count = 25;
    for(int i = 0; i < u_fractal_iteration; i++)
    {
        vec2 zi = zn;
        // Расчетная формула
        zn.x = pow(zi.x,2.0)-pow(zi.y,2.0);
        zn.y = 2.0*zi.x*zi.y;
        zn += position_;
    }
    // Определяем цвет точки, задав произвольные условия
    // для каждой компоненты цвета
    return vec4(length(zn),
                length(zn-position_),
                length(zn-position_)/length(position_),
                1.0);
}

/*
    Функция, которая рисует множество Жюлиа
    position - позиция точки экрана, для которой устанавливаем цвет;
    shift - смещение позиции относительно центра после масштабирования;
    scale - масштаб.
*/
vec4 JuliaSet(vec2 position, vec2 shift, vec2 scale)
{
    // Новая позиция с учетом масштабирования и смещения
    vec2 position_ = (vec2(0.5)-position)*scale - shift;
    vec2 zn= position_;
    vec2 c = vec2(-0.8, 0.18);

    // Количество итераций в цикле влияет на четкость изображения
    const int count = 50;
    for(int i = 0; i < count; i++)
    {
        vec2 zi = zn;
        // Расчетная формула
        zn.x = pow(zi.x,2.0)-pow(zi.y,2.0);
        zn.y = 2.0*zi.x*zi.y;
        zn += c;
    }
    // Определяем цвет точки, задав произвольные условия
    // для каждой компоненты цвета
    return vec4(1.0-length(zn),
                1.0-length(zn-position_),
                1.0-length(zn-position_)/length(position_),
                1.0);
}

void main()
{

    vec2 position = gl_FragCoord.xy/u_resolution.xy;
    
    if(u_fractal_type==1)
        gl_FragColor = MandelbrotSet(position,u_shift,u_scale);
    else
        gl_FragColor = JuliaSet(position,u_shift,u_scale);
    // gl_FragColor = MandelbrotSet(position,vec2(1,0),vec2(3,3));
    // gl_FragColor = JuliaSet(position,vec2(0,0),vec2(3,3));
}
