#version 330 core

layout(location = 0) in vec3 aPos;  // Cube vertices
layout(location = 1) in vec3 instancePos; // Voxel position
layout (location = 2) in vec4 instanceColor; // Voxel color

out vec4 voxelColor; // Pass to fragment shader

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(aPos + instancePos, 1.0);
    voxelColor = instanceColor;
}