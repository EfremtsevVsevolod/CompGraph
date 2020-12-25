#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 FragPos;

uniform mat4 shadow_view[6];

void main() {
    for (int idx = 0; idx < 6; ++idx) {
        gl_Layer = idx;
        for (int jdx = 0; jdx < 3; ++jdx) {
            FragPos = gl_in[jdx].gl_Position;
            gl_Position = shadow_view[idx] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}