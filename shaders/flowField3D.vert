#version 330 core

layout(location = 0) in vec3 aPos; // Cube vertex position

out vec3 TexCoords; // Texture coordinates for the fragment shader

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    // Calculate texture coordinates based on vertex position
    TexCoords = aPos + vec3(0.5); // Shift to [0, 1] range

    // Transform the vertex position
    gl_Position = vec4(aPos, 1.0); //projection * view * model * 
}