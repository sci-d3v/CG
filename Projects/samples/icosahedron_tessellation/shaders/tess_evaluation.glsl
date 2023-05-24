#version 410 core

layout(triangles,equal_spacing,cw)in;

in vec3 color_tc[];
out vec3 color_te;

void main(void)
{
    color_te = gl_TessCoord.x*color_tc[0]+
                    gl_TessCoord.y*color_tc[1]+
                    gl_TessCoord.z*color_tc[2];

    vec3 coordinate=gl_TessCoord.x*gl_in[0].gl_Position.xyz+
                    gl_TessCoord.y*gl_in[1].gl_Position.xyz+
                    gl_TessCoord.z*gl_in[2].gl_Position.xyz;


    coordinate=normalize(coordinate);
    gl_Position=vec4(coordinate,1.);

}