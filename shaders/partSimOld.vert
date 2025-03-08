#version 430 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in float activated;

out vec4 vertexColor; // Output color to the fragment shader

void main() {
    gl_PointSize = 3.0;
    gl_Position = vec4(inPosition.xyz, 1.0);
    vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
    if (activated < 0.0) {
        vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}