#version 330 core

// we take triangles in, spit triangles out
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// carry the chosen color down to the fragment shader
out vec3 gsColor;

void main() {
    // grab the clip-space positions of the three verts
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;

    // compute the face-normal in clip space
    vec3 fn = normalize( cross(p1 - p0, p2 - p0) );
    vec3 an = abs(fn);

    // pick the axis with the largest absolute normal component
    if (an.x >= an.y && an.x >= an.z)     gsColor = vec3(1,0,0);
    else if (an.y >= an.x && an.y >= an.z) gsColor = vec3(0,1,0);
    else                                   gsColor = vec3(0,0,1);

    // re-emit the triangle, carrying our flat color
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
