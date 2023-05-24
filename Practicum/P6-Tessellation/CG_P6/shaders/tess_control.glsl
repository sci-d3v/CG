#version 410 core

layout(vertices=3)out;
in vec3 color_v[];
out vec3 color_tc[];

uniform int parameter_tc;

void main(void)
{
	color_tc[gl_InvocationID]=color_v[gl_InvocationID];
	vec4 coordinate=gl_in[gl_InvocationID].gl_Position;
	gl_out[gl_InvocationID].gl_Position=coordinate;

	if(gl_InvocationID==0){
			gl_TessLevelInner[0]=parameter_tc;
			gl_TessLevelInner[1]=parameter_tc;
			gl_TessLevelOuter[0]=parameter_tc;
			gl_TessLevelOuter[1]=parameter_tc;
			gl_TessLevelOuter[2]=parameter_tc;
	}
}