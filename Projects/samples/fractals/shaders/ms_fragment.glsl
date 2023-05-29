#version 330 core

uniform vec2 u_resolution;
uniform vec2 u_shift;
uniform vec2 u_scale;

uniform int u_fractal_iteration;

/*
Функция, которая рисует множество Мандельброта
position - позиция точки экрана, для которой устанавливается цвет;
shift - смещение позиции относительно центра после масштабирования;
scale - масштаб.
*/
vec4 MandelbrotSet(vec2 position,vec2 shift,vec2 scale){
    // Новая позиция с учетом масштабирования и смещения
    vec2 position_=(position-vec2(.5))*scale-shift;
    // Для множества Мандельброта всегда z0=0
    vec2 zn=vec2(0);
    
    // Количество итераций в цикле влияет на четкость изображения
    // const int count = 25;
    for(int i=0;i<u_fractal_iteration;++i){
        vec2 zi=zn;
        // Расчетная формула
        zn.x=pow(zi.x,2.)-pow(zi.y,2.);
        zn.y=2.*zi.x*zi.y;
        zn+=position_;
    }
    // Определяем цвет точки, задав произвольные условия
    // для каждой компоненты цвета
    
    // if(length(zn)<1.)
    //     return vec4(1.,0,0,1.);
    // else
    //     return vec4(0,0,1,1.);
    
    return vec4(length(zn),length(zn-position_),length(zn-position_)/length(position_),1.);
    // return vec4(1.-length(zn),1.-length(zn-position_),1.-length(zn-position_)/length(position_),1.);
    // float p = 100.;
    // return vec4(1. - length(zn)/p ,1. - length(zn-position_)/p,
    // // 1.-length(zn-position_)/length(position_),
    // 0.0,
// 1.);
}

void main(){
vec2 position=gl_FragCoord.xy/u_resolution.xy;
gl_FragColor=MandelbrotSet(position,u_shift,u_scale);
// gl_FragColor = MandelbrotSet(position,vec2(1,0),vec2(3,3));
}