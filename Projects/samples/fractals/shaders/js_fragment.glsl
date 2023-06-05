#version 330 core

uniform vec2 u_resolution;
uniform vec2 u_shift;
uniform vec2 u_scale;

uniform int u_fractal_iteration;
uniform vec2 u_fractal_constant;

/*
Функция, которая рисует множество Жюлиа
position - позиция точки экрана, для которой устанавливаем цвет;
shift - смещение позиции относительно центра после масштабирования;
scale - масштаб.
*/
vec4 JuliaSet(vec2 position,vec2 shift,vec2 scale,vec2 constant){
    // Новая позиция с учетом масштабирования и смещения
    vec2 z0=(vec2(.5)-position)*scale-shift;
    vec2 zn=z0;
    // vec2 c = vec2(-0.8, 0.18);
    
    // Количество итераций в цикле влияет на четкость изображения
    // const int count=50;
    
    for(int i=0;i<u_fractal_iteration;++i){
        vec2 zi=zn;
        // Расчетная формула
        zn.x=pow(zi.x,2.)-pow(zi.y,2.);
        zn.y=2.*zi.x*zi.y;
        zn+=constant;
    }
    // Определяем цвет точки, задав произвольные условия
    // для каждой компоненты цвета
    return vec4(1.-length(zn),1.-length(zn-z0),1.-length(zn-z0)/length(z0),1.);
}

void main(){
    vec2 position=gl_FragCoord.xy/u_resolution.xy;
    gl_FragColor=JuliaSet(position,u_shift,u_scale,u_fractal_constant);
    // gl_FragColor = JuliaSet(position,vec2(0,0),vec2(3,3),vec2(-.8,.18));
}
