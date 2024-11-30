#version 410 core

layout(triangles)in;
layout(line_strip,max_vertices=4)out;

in vec3 color_te[];
out vec3 color_g;

void main()
{
	for(int i=0;i<gl_in.length();i++)
	{
		color_g=color_te[i];
		vec3 coordinate=gl_in[i].gl_Position.xyz;
		gl_Position=vec4(coordinate,1.2);
		EmitVertex();
	}
	color_g=color_te[0];
	vec3 coordinate=gl_in[0].gl_Position.xyz;
	gl_Position=vec4(coordinate,1.2);
	EmitVertex();
	
	EndPrimitive();
}