#version 430 core

//out vec4 fragColor;
in vec4 vertexColor; // Input from the vertex shader

out vec4 FragColor; // Final color output

void main() {
    //fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = vertexColor;
}