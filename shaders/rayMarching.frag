#version 330 core

out vec4 FragColor;
in vec3 localPos;

uniform sampler3D my3DTexture;
uniform vec3 cameraPos;
uniform vec3 rayDir;       // Becomes normalize(vec3(1, -1, -1))
uniform vec3 volumeDim;    // Size of the volume in voxels
uniform mat4 invViewProj;

vec3 signVec(vec3 v) {
    return vec3(
        v.x > 0.0 ? 1.0 : (v.x < 0.0 ? -1.0 : 0.0),
        v.y > 0.0 ? 1.0 : (v.y < 0.0 ? -1.0 : 0.0),
        v.z > 0.0 ? 1.0 : (v.z < 0.0 ? -1.0 : 0.0)
    );
}

vec3 mixThreeColors(vec3 c1, vec3 c2, vec3 c3, float t) {
    if (t < 0.8) {
        return mix(c1, c2, pow(t * 1.25, 4));  // Blend between c1 and c2 for t in [0, 0.5]
    } else if (t < 0.95) {
        return mix(c2, c3, (t - 0.8) * 6.67); // Blend between c2 and c3 for t in [0.5, 1.0]
    } else {
        return vec3(1.0); // Return white for t >= 0.95
    }
}

void main() {
    int maxSteps = int(volumeDim.x + volumeDim.z + volumeDim.z); // You meant z twice

    // Convert ray origin to voxel space
    vec3 rayPos = (localPos * 0.5 + 0.5) * volumeDim; // [-1, 1] -> [0, 1] -> [0, volumeDim]
            
    // Lodev.org also explains this additional optimistaion (but it's beyond scope of video)
    vec3 deltaDist = abs(vec3(1.0) / rayDir);

    //olc::vf2d deltaDist = { sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)), sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) };
  
    vec3 unitStep = signVec(rayDir);

    // Voxel we're in
    ivec3 voxel = ivec3(floor(rayPos));
    vec3 sideDist = unitStep * (vec3(voxel) + 0.5 * (1.0 + unitStep) - rayPos) * deltaDist;

    float fDistance = 0.0f;
    vec4 color = vec4(0.0);
    float alphaThreshold = 0.999;

    vec3 orange = vec3(0.965, 0.627, 0.227);
    vec3 yellow = vec3(0.988, 0.96,  0.345);
    vec3 white  = vec3(1.0,   1.0,   1.0);

    for (int i = 0; i < maxSteps; ++i) {

        // Sample voxel color
        vec3 texCoord = vec3(voxel + 0.5) / volumeDim;
        vec4 voxelColor = texture(my3DTexture, texCoord);
        float alpha = voxelColor.a;

        // Alpha blending (front-to-back)
        color.rgb += (1.0 - color.a) * orange * alpha;
        color.a += (1.0 - color.a) * alpha;

        if (color.a > alphaThreshold)
            break;
            
        // Walk along shortest path, DDA step
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            fDistance = sideDist.x;
            sideDist.x += deltaDist.x;
            voxel.x += int(unitStep.x);
        } else if (sideDist.y < sideDist.z) {
            fDistance = sideDist.y;
            sideDist.y += deltaDist.y;
            voxel.y += int(unitStep.y);
        } else {
            fDistance = sideDist.z;
            sideDist.z += deltaDist.z;
            voxel.z += int(unitStep.z);
        }

        // Check bounds
        if (any(lessThan(voxel, ivec3(0))) || any(greaterThanEqual(voxel, ivec3(volumeDim))))
            break;
    }

    vec3 finalColor = mixThreeColors(orange, yellow, white, color.a); // mix toward white
    FragColor = vec4(finalColor, color.a);
}