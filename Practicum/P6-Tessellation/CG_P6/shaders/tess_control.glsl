#version 410 core

layout(vertices=3)out;

void main(void)
{
	vec4 coordinate=gl_in[gl_InvocationID].gl_Position;
	gl_out[gl_InvocationID].gl_Position=coordinate;
	
	int inner=4;
	int outer=4;
	if(gl_InvocationID==0){
		gl_TessLevelInner[0]=inner;
		gl_TessLevelOuter[0]=outer;
		gl_TessLevelOuter[1]=outer;
		gl_TessLevelOuter[2]=outer;
	}
}