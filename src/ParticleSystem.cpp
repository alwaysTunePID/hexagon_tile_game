#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ParticleSystem.h"
#include "Transformations.h"

ParticleSystem::ParticleSystem()
    : angle{}, time{ 0.f }, m_scale{2.f}, vao{}, vbo{}, ebo{}, instanceVBO{}, colorVBO{}, flowFieldTexture{},
    fadeProgram{}, flowFieldProgram{}, flowFieldRenderProgram{}, psTexture{}, blurPsTexture{},
    blurProgram{}, particleSystemProgram{}, ballProgram{}, particleBuffer{}
{
    float cubeVertices[] = {
        -1.0, -1.0,  1.0, //0
         1.0, -1.0,  1.0, //1
        -1.0,  1.0,  1.0, //2
         1.0,  1.0,  1.0, //3
        -1.0, -1.0, -1.0, //4
         1.0, -1.0, -1.0, //5
        -1.0,  1.0, -1.0, //6
         1.0,  1.0, -1.0  //7
    };

    // Cube indices
    unsigned int indices[] = {
        //Top
        2, 6, 7,
        2, 3, 7,

        //Bottom
        0, 4, 5,
        0, 1, 5,

        //Left
        0, 2, 6,
        0, 4, 6,

        //Right
        1, 3, 7,
        1, 5, 7,

        //Front
        0, 2, 3,
        0, 1, 3,

        //Back
        4, 6, 7,
        4, 5, 7
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Upload cube vertices
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Upload cube indices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* This was for the cube grid thing
    ************************************
    // Generate instance positions for voxel grid
    std::vector<glm::vec3> voxelPositions;
    for (int x = 0; x < GRID_SIZE; ++x)
        for (int y = 0; y < GRID_SIZE; ++y)
            for (int z = 0; z < GRID_SIZE; ++z)
                voxelPositions.push_back(glm::vec3(x, y, z));

    // Upload instance positions
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::vec3), voxelPositions.data(), GL_STATIC_DRAW);

    // Set instance attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // Update per instance

    // Generate instance colors for voxels
    std::vector<glm::vec4> voxelColors;
    for (int x = 0; x < GRID_SIZE; ++x)
        for (int y = 0; y < GRID_SIZE; ++y)
            for (int z = 0; z < GRID_SIZE; ++z)
            {
                if (pow((x - GRID_SIZE/2), 2) + pow((y - GRID_SIZE/2), 2) + pow((z - GRID_SIZE/2), 2) < 20*20)
                    voxelColors.push_back(glm::vec4((float)x / GRID_SIZE, (float)y / GRID_SIZE, (float)z / GRID_SIZE, 1.0f));
                else
                    voxelColors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)); // Transparent  0.005f
            }

    // Upload instance colors
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, voxelColors.size() * sizeof(glm::vec4), voxelColors.data(), GL_STATIC_DRAW);

    // Set instance color attribute
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // Update per instance
    */

    glBindVertexArray(0);

    // Store flow field as a texture
    glGenTextures(1, &flowFieldTexture);
    glBindTexture(GL_TEXTURE_3D, flowFieldTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, flowTextureRes.x, flowTextureRes.y, flowTextureRes.z, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // Store particle positions as a texture
    glGenTextures(1, &psTexture);
    glBindTexture(GL_TEXTURE_3D, psTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, psTextureRes.x, psTextureRes.y, psTextureRes.z, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // Store previous iteration particle positions as a texture
    glGenTextures(1, &prevPosTexture);
    glBindTexture(GL_TEXTURE_3D, prevPosTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, psTextureRes.x, psTextureRes.y, psTextureRes.z, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // Store previous iteration particle positions as a texture
    glGenTextures(1, &blurPsTexture);
    glBindTexture(GL_TEXTURE_3D, blurPsTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, psTextureRes.x, psTextureRes.y, psTextureRes.z, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    fadeProgram      = createComputeShader("../../../shaders/fadeTrail3D.comp");
    flowFieldProgram = createComputeShader("../../../shaders/flowField3D.comp");
    flowFieldRenderProgram = createProgram(
        loadShader("../../../shaders/rayMarching.vert", GL_VERTEX_SHADER),
        loadShader("../../../shaders/rayMarching.frag", GL_FRAGMENT_SHADER));

    particleSystemProgram = createComputeShader("../../../shaders/partSim3D.comp");
    blurProgram           = createComputeShader("../../../shaders/blur3D.comp");
    ballProgram           = createComputeShader("../../../shaders/ball3D.comp");

    initParticles();
}

ParticleSystem::~ParticleSystem()
{}

void ParticleSystem::initParticles()
{
    // Create OpenGL buffer for particles
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES_3D * sizeof(Particle3D), nullptr, GL_DYNAMIC_DRAW); // nullptr instead of particles.data()?
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

    // Create VAO for rendering
    //glGenVertexArrays(1, &vao);
}

void ParticleSystem::fadeParticleTrail(float dt) {

    glUseProgram(fadeProgram);

    // Bind the 3D texture as an image
    glBindImageTexture(0, psTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, blurPsTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Set uniforms
    glUniform3i(glGetUniformLocation(fadeProgram, "textureRes"), flowTextureRes.x, flowTextureRes.y, flowTextureRes.z);
    glUniform1f(glGetUniformLocation(fadeProgram, "deltaTime"), dt);

    // Dispatch compute shader
    glm::ivec3 groupSize = flowTextureRes / 8; // 32*8=256 //  (GRID_SIZE + 7) / 8; // Match local_size_x/y/z in compute shader
    glDispatchCompute(groupSize.x, groupSize.y, groupSize.z);

    // Ensure the texture is updated before rendering
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUseProgram(0);

    std::swap(blurPsTexture, psTexture);
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearTexImage(blurPsTexture, 0, GL_RGBA, GL_FLOAT, clearColor);
}

void ParticleSystem::updateFlowField(float dt) {

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearTexImage(flowFieldTexture, 0, GL_RGBA, GL_FLOAT, clearColor);

    glUseProgram(flowFieldProgram);

    // Bind the 3D texture as an image
    glBindImageTexture(0, flowFieldTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Set uniforms
    glUniform3i(glGetUniformLocation(flowFieldProgram, "textureRes"), flowTextureRes.x, flowTextureRes.y, flowTextureRes.z);
    glUniform1f(glGetUniformLocation(flowFieldProgram, "deltaTime"), dt);

    // Dispatch compute shader
    glm::ivec3 groupSize = flowTextureRes / 8; // 32*8=256 //  (GRID_SIZE + 7) / 8; // Match local_size_x/y/z in compute shader
    glDispatchCompute(groupSize.x, groupSize.y, groupSize.z);

    // Ensure the texture is updated before rendering
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUseProgram(0);
}

void ParticleSystem::updateParticles(float dt) {

    time += dt;
    if (time > 10000.0f) {
        time = 0.0f;
    }

    glUseProgram(particleSystemProgram);
    // Bind the texture to an image unit before dispatching the compute shader
    glBindImageTexture(0, flowFieldTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, prevPosTexture,   0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(2, psTexture,        0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // TODO: Optimize this:
    // // Get uniform location once (after linking the shader)
    // GLint circleRadiusLoc = glGetUniformLocation(computeProgram, "circleRadius");
    // if (cRadius != prevCRadius) {
    //    glUniform1f(circleRadiusLoc, cRadius);
    //    prevCRadius = cRadius;  // Store the last value
    // }

    // Temp solution
    static int isInitState = 1;

    glUniform1f(glGetUniformLocation(particleSystemProgram, "deltaTime"), dt);
    glUniform1f(glGetUniformLocation(particleSystemProgram, "time"), time);
    glUniform2f(glGetUniformLocation(particleSystemProgram, "uResolution"), (float)WIDTH2, (float)HEIGHT2);
    glUniform3i(glGetUniformLocation(particleSystemProgram, "textureRes"), psTextureRes.x, psTextureRes.y, psTextureRes.z);
    glUniform1i(glGetUniformLocation(particleSystemProgram, "isInitState"), isInitState);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    // Maybe change to this
    //int numWorkGroups = (NUM_PARTICLES + 255) / 256;
    glDispatchCompute(NUM_PARTICLES_3D / 256, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Temp removed.
    //float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    //glClearTexImage(prevPosTexture, 0, GL_RGBA, GL_FLOAT, clearColor);

    // Unbind the image texture
    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(2, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glUseProgram(0);

    if (isInitState) {
        isInitState = 0; // Set to false after the first update
    }
}

void ParticleSystem::blurParticles(float dt) {

    glUseProgram(blurProgram);

    // Bind the 3D texture as an image
    glBindImageTexture(0, psTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, blurPsTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Set uniforms
    glUniform3i(glGetUniformLocation(blurProgram, "textureRes"), psTextureRes.x, psTextureRes.y, psTextureRes.z);
    glUniform1f(glGetUniformLocation(blurProgram, "deltaTime"), dt);

    // Dispatch compute shader
    glm::ivec3 groupSize = psTextureRes / 8; // 32*8=256 //  (GRID_SIZE + 7) / 8; // Match local_size_x/y/z in compute shader
    glDispatchCompute(groupSize.x, groupSize.y, groupSize.z);

    // Ensure the texture is updated before rendering
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUseProgram(0);

    std::swap(blurPsTexture, psTexture);
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearTexImage(blurPsTexture, 0, GL_RGBA, GL_FLOAT, clearColor);
}

void ParticleSystem::updateBall(float dt) {

    glUseProgram(ballProgram);

    // Bind the 3D texture as an image
    glBindImageTexture(0, psTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Set uniforms
    glUniform3i(glGetUniformLocation(ballProgram, "textureRes"), psTextureRes.x, psTextureRes.y, psTextureRes.z);
    glUniform1f(glGetUniformLocation(ballProgram, "deltaTime"), dt);

    // Dispatch compute shader
    glm::ivec3 groupSize = psTextureRes / 8; // 32*8=256 //  (GRID_SIZE + 7) / 8; // Match local_size_x/y/z in compute shader
    glDispatchCompute(groupSize.x, groupSize.y / 2, groupSize.z);

    // Ensure the texture is updated before rendering
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUseProgram(0);
}

void ParticleSystem::updatePosAndScale(displayInput& camera)
{
    m_scale = camera.zoom / (20.f * 2.f);
}

void ParticleSystem::draw() {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);        // Cull back-facing triangles
    //glFrontFace(GL_CCW);        // Counter-clockwise winding is front-facing (default)

    // Temp here
    float orthoSize = 2.0f; // this controls how zoomed in/out you are
    float aspect = static_cast<float>(WIDTH2) / static_cast<float>(HEIGHT2);
    float orthoWidth = orthoSize * aspect;
    float orthoHeight = orthoSize; // Keep this fixed to avoid stretching

    angle += 1.0f;

    glm::vec3 cameraPos = glm::vec3(2.0f, 2.0f, 2.0f); // Pick a point above and diagonally back
    //glm::vec3 cameraPos = glm::vec3(glm::inverse(view)[3]); // Extract translation column

    // 1. Orthographic projection
    glm::mat4 projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -10.0f, 10.0f); //glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -100.0f, 300.0f);

    // 2. Isometric view matrix
    glm::mat4 view = glm::rotate(glm::mat4(1.0f), glm::radians(35.264f), glm::vec3(1, 0, 0)); // x-axis glm::translate(glm::mat4(1.0f), glm::vec3(-gridSize / 2, -gridSize / 2, -30.0f));
    view = glm::rotate(view, glm::radians(45.0f), glm::vec3(0, 1, 0)); // y-axis
    // Model matrix: Translate to origin -> Rotate -> Translate back
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(gridSize / 2, gridSize / 2, gridSize / 2)); // Translate to center

    // 3. Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_scale, 2.0f * m_scale, m_scale));

    // 4. Inverse View-Projection for ray direction in shader
    glm::mat4 viewProj = projection * view;
    glm::mat4 invViewProj = glm::inverse(viewProj);

    glm::vec3 rayDir = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, -1, 0));
    rayDir = glm::normalize(rayDir);

    glUseProgram(flowFieldRenderProgram);

    glUniformMatrix4fv(glGetUniformLocation(flowFieldRenderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(flowFieldRenderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(flowFieldRenderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(flowFieldRenderProgram, "invViewProj"), 1, GL_FALSE, glm::value_ptr(invViewProj));
    glUniform3fv(glGetUniformLocation(flowFieldRenderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(flowFieldRenderProgram, "rayDir"), 1, glm::value_ptr(rayDir));
    glUniform3fv(glGetUniformLocation(flowFieldRenderProgram, "volumeDim"), 1, glm::value_ptr(glm::vec3(psTextureRes)));

    // Bind the 3D texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, psTexture);
    glUniform1i(glGetUniformLocation(flowFieldRenderProgram, "my3DTexture"), 0);

    // Render the cube geometry
    glBindVertexArray(vao);
    //glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, GRID_SIZE * GRID_SIZE * GRID_SIZE);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
    //glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);

    // TODO: Move this!!!
    // Set psTexture as prevPosTexture for next frame
    //std::swap(prevPosTexture, psTexture);
}

GLuint ParticleSystem::loadShader(const std::string& filePath, GLenum type)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source{ buffer.str() };

    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation error: " << log << "\n";
    }

    return shader;
}

GLuint ParticleSystem::createComputeShader(const std::string& filePath) {
    GLuint computeShader = loadShader(filePath, GL_COMPUTE_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << "\n";
    }

    glDeleteShader(computeShader);
    return program;
}

GLuint ParticleSystem::createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << "\n";
    }

    // Validate program
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program validation error: " << log << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}