#version 410 core

// layout(triangles,equal_spacing,cw)in;
layout(quads,cw)in;

in vec3 color_tc[];
out vec3 color_te;

void main(void){

    float u=gl_TessCoord.x;
    float v=gl_TessCoord.y;
    vec2 p1=mix(gl_in[0].gl_Position.xy,gl_in[1].gl_Position.xy,u);
    vec2 p2=mix(gl_in[3].gl_Position.xy,gl_in[2].gl_Position.xy,u);

    gl_Position=vec4(mix(p1,p2,v),0.,1.);
    // gl_Position=vec4(normalize( vec4(mix(p1,p2,v),0.,1.)).xyz,1.);
  
    
    color_te=gl_TessCoord.x*color_tc[0]+
    gl_TessCoord.y*color_tc[1]+
    gl_TessCoord.z*color_tc[2];


    // Calculate the interpolation parameters
    // float u = gl_TessCoord.x;
    // float v = gl_TessCoord.y;
    // float w = 1.0 - u - v;
    
    // // Interpolate the position of the current vertex
    // vec3 p = u * gl_in[0].gl_Position.xyz + v * gl_in[1].gl_Position.xyz + w * gl_in[2].gl_Position.xyz;
    // gl_Position = vec4(p, 1.0);
    
    // // Interpolate the color of the current vertex
    // color_te = u * color_tc[0] + v * color_tc[1] + w * color_tc[2];
}