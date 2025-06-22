#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Enums.h"

// Voxel Grid Dimensions
//const int GRID_SIZE = 16; // GRID_SIZE^3 volume of cubes
const glm::ivec3 flowTextureRes = glm::ivec3(256, 512, 256);

// Particle System Dimensions
const glm::ivec3 psTextureRes = glm::ivec3(256, 512, 256);

// Configure number of particles in the system
const int NUM_PARTICLES_3D = 100*256; // Needs to be even work group size

// Particle data structure
struct Particle3D {
    float x_n, y_n, z_n, w_n; // Position [-1, 1] + padding
    float x, y, z, w;         // Simulation local position + padding
    float vx, vy, vz, vw;     // Simulation local Velocity + padding
    float lifeTime, totalLifeTime;
    int activated;
    float p3;
};

class ParticleSystem
{
private:
    float angle;
    float time;
    float m_scale;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint instanceVBO;
    GLuint colorVBO;
    GLuint flowFieldTexture;
    GLuint fadeProgram;
    GLuint flowFieldProgram;
    GLuint flowFieldRenderProgram;
    GLuint psTexture;
    GLuint prevPosTexture;
    GLuint blurPsTexture;
    GLuint particleSystemProgram;
    GLuint blurProgram;
    GLuint ballProgram;
    GLuint particleBuffer;

public:
    ParticleSystem();
    ~ParticleSystem();

    void initParticles();
    void fadeParticleTrail(float dt);
    void updateFlowField(float dt);
    void updateParticles(float dt);
    void blurParticles(float dt);
    void updateBall(float dt);
    void updatePosAndScale(displayInput& camera);
    void draw();

    // Shader loading utility
    GLuint loadShader(const std::string& filePath, GLenum type);
    GLuint createComputeShader(const std::string& filePath);
    GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

};

#endif