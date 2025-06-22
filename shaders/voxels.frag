#version 330 core

in vec4 voxelColor; // Received from vertex shader
out vec4 FragColor;

void main() {
    FragColor = voxelColor;
}