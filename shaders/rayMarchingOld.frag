#version 330 core

out vec4 FragColor;
in vec3 localPos;

uniform sampler3D my3DTexture;
uniform vec3 cameraPos;
uniform vec3 rayDir; // Becomes normalize(vec3(1, -1, -1))
uniform mat4 invViewProj;

#define MAX_STEPS 200
#define MIN_STEP 0.005
#define MAX_STEP 0.05
#define DENSITY_THRESHOLD 0.02

void main()
{
    // Convert localPos (-1 to 1) to world space
    vec4 worldPos = invViewProj * vec4(localPos, 1.0);
    //vec3 rayDir = normalize(worldPos.xyz - cameraPos);

    vec3 rayPos = localPos * 0.5 + 0.5; // from [-1, 1] to [0, 1]
    vec4 accumulatedColor = vec4(0.0);
    float t = 0.0;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        vec3 samplePos = rayPos + rayDir * t;
        if (any(lessThan(samplePos, vec3(0.0))) || any(greaterThan(samplePos, vec3(1.0))))
            break;

        //samplePos = round(samplePos * 16) / 16.0;
        vec4 sample = texture(my3DTexture, samplePos);

        // Accumulate with alpha blending
        accumulatedColor.rgb += (1.0 - accumulatedColor.a) * sample.rgb * sample.a;
        accumulatedColor.a += sample.a * (1.0 - accumulatedColor.a);

        if (accumulatedColor.a >= 0.95)
            break;

        // Adaptive step size
        float stepSize = 0.005; // mix(MAX_STEP, MIN_STEP, clamp(sample.a / DENSITY_THRESHOLD, 0.0, 1.0));
        t += stepSize;
    }

    FragColor = accumulatedColor;
}