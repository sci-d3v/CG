#version 410 core

layout(triangles)in;
layout(line_strip,max_vertices=4)out;

in vec3 color_te[];
out vec3 color_g;

// uniform mat4 matModel;

// Function to transform cube coordinates to the surface of a sphere
vec3 cubeToSphere(vec3 cubePosition,float radius){
	// Step 1: Convert to spherical coordinates (θ, φ)
	float theta=atan(cubePosition.y,cubePosition.x);
	float phi=acos(cubePosition.z/radius);
	
	// Step 2: Convert back to Cartesian coordinates
	float x=radius*sin(phi)*cos(theta);
	float y=radius*sin(phi)*sin(theta);
	float z=radius*cos(phi);
	
	return vec3(x,y,z);
}

void main(){
	// int k=3;
	// if(	gl_in[0].gl_Position.x == gl_in[1].gl_Position.x &&
	// 	gl_in[1].gl_Position.x == gl_in[2].gl_Position.x)
	// 	k = 1;
	// else if(gl_in[0].gl_Position.y == gl_in[1].gl_Position.y &&
	// 		gl_in[1].gl_Position.y == gl_in[2].gl_Position.y)
	// 	k = 2;
	
	// for(int i=0;i<gl_in.length();++i)
	// 	for(int j=0;j<3;++j)
	// 	{
		// 		if(k==j)
		
	// 	}
	
	float sphereRadius=1.;
	for(int i=0;i<gl_in.length();++i){
		color_g=color_te[i];
		vec3 coordinate=gl_in[i].gl_Position.xyz;
		// coordinate=normalize(coordinate);
		// Transform cube coordinates to the surface of the sphere
		// coordinate=cubeToSphere(coordinate,sphereRadius);
		// coordinate = vec3(coordinate.xy,1.0);
		// coordinate = coordinate*sqrt(coordinate.x*coordinate.x+coordinate.y*coordinate.y+coordinate.z);
		// coordinate=normalize(vec4(coordinate,.025)).xyz;
		
		gl_Position=vec4(coordinate,1.5);
		
		// gl_FrontColor = gl_FrontColorIn[i];
		// gl_FrontColor.a = 0.5; // Set transparency if needed
		// // Assign depth value based on vertex position
		// gl_FragDepth = gl_in[i].gl_Position.z;
		// gl_Position=vec4(coordinate,1)*matModel;
		EmitVertex();
	}
	color_g=color_te[0];
	vec3 coordinate=gl_in[0].gl_Position.xyz;
	// coordinate=normalize(coordinate);
	// coordinate=cubeToSphere(coordinate,sphereRadius);
	// coordinate = coordinate*sqrt(coordinate.x*coordinate.x+coordinate.y*coordinate.y+coordinate.z*coordinate.z);
	// coordinate=normalize(vec4(coordinate,.025)).xyz;
	
	gl_Position=vec4(coordinate,1.5);
	// gl_FrontColor=gl_FrontColorIn[i];
	// gl_FrontColor.a=.5;// Set transparency if needed
	// // Assign depth value based on vertex position
	// gl_FragDepth=gl_in[0].gl_Position.z;
	// gl_Position=vec4(coordinate,1)*matModel;
	EmitVertex();
	EndPrimitive();
	
}