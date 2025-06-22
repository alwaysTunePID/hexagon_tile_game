#version 330 core

in vec3 TexCoords; // 3D texture coordinates

out vec4 FragColor;

uniform sampler3D my3DTexture;

void main()
{
    // Sample the 3D texture to get the voxel color
    vec4 voxelColor = texture(my3DTexture, TexCoords);

    // Discard transparent voxels
    if (voxelColor.a < 0.01) {
        discard;
    }

    FragColor = voxelColor;
}