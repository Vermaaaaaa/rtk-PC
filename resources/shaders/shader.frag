#version 330 core

in  vec3 gsColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(gsColor, 1.0);
}
