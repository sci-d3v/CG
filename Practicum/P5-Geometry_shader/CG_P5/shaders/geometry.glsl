#version 400 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 99) out;

in VertexData
{
    vec3 color;
}
extra_data[];

out vec3 color_f;

//Sierpinski triangle
void main()
{
    const int n = 3; // recursion steps
    const int q = 3; // recursive calls in one recursive step

    if (n == 0)
    {
        // Drawing a simple base case
        for (int i = 0; i < gl_in.length(); i++)
        {
            gl_Position = gl_in[i].gl_Position;
            color_f = extra_data[i].color;
            EmitVertex();
        }
        EndPrimitive();
    }
    else
    {
        int p = 0;
        vec4 vertices[q]; // Vertex array
        vec3 colors[q];   // Color array
        int b;
        while (p < int(pow(q, n)))
        {
            // A recursive step
            b = p;
            for (int i = 0; i < q; i++)
            {
                vertices[i] = gl_in[i].gl_Position;
                colors[i] = extra_data[i].color;
            }
            // Searching next triangle
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < q; j++)
                    if (b % 3 != j)
                    {
                        vertices[j] = (vertices[b % 3] + vertices[j]) / 2.;
                        colors[j] = (colors[b % 3] + colors[j]) / 2.;
                    }
                b = b / 3;
            }
            // Drawing one of triangles
            for (int i = 0; i < q; i++)
            {
                gl_Position = vertices[i];
                color_f = colors[i];
                EmitVertex();
            }
            EndPrimitive();

            // Next recursive step
            p += 1;
        }
    }
} //\0