#version 410 core

layout(triangles)in;
layout(line_strip,max_vertices=8)out;

void main()
{
	for(int i=0;i<gl_in.length();i++)
	{
		vec3 coordinate=gl_in[i].gl_Position.xyz;
		gl_Position=vec4(coordinate,2.);
		EmitVertex();
	}
	
	EndPrimitive();
}