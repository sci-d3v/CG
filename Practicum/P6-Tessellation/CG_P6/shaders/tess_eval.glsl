#version 410 core

layout(triangles,equal_spacing,cw)in;

void main(void)
{
    vec3 coordinate=gl_TessCoord.x*gl_in[0].gl_Position.xyz+
                    gl_TessCoord.y*gl_in[1].gl_Position.xyz+
                    gl_TessCoord.z*gl_in[2].gl_Position.xyz;
    
    coordinate=normalize(coordinate);
    gl_Position=vec4(coordinate,1.);
    
}