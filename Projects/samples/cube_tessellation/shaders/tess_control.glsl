#version 430 core

layout(vertices=4)out;
in vec3 color_v[];
out vec3 color_tc[];

uniform int parameter_tc;

void main(void){
	color_tc[gl_InvocationID]=color_v[gl_InvocationID];
	vec4 position = gl_in[gl_InvocationID].gl_Position;
	// position = vec4(normalize(position.xyz),1.5);
	gl_out[gl_InvocationID].gl_Position= position;

	if(gl_InvocationID==0){
			gl_TessLevelInner[0]=parameter_tc;
			gl_TessLevelInner[1]=parameter_tc;
			gl_TessLevelOuter[0]=parameter_tc;
			gl_TessLevelOuter[1]=parameter_tc;
			gl_TessLevelOuter[2]=parameter_tc;
			gl_TessLevelOuter[3]=parameter_tc;
	}
}