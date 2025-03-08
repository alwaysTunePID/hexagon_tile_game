#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D psTexture;

void main()
{
    // TODO: Temp hardcoding
    vec2 offset = vec2(1.0 / 256.0, 1.0 / 256.0); // Blur intensity (adjust based on resolution)
    vec4 result = vec4(0.0);
    vec4 pixel = vec4(0.0);
    
    // Gaussian kernel weights
    float weights[4] = float[](0.227027, 0.1945946, 0.1216216, 0.054054);

    // Use this or the other section for blur. But don't think they work yet for scaled textures.
    // Apply blur using a weighted sum
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            vec2 sampleOffset = TexCoords + vec2(offset.x * float(i), offset.y * float(j));
            result += texture(psTexture, sampleOffset).rgba * ((weights[abs(i)] * weights[abs(j)]) * 2.0);
        }
    }

    // This is the other way to apply blur
    //int nColors = 0;
    //for (int i = -2; i <= 2; i++) {
    //    for (int j = -2; j <= 2; j++) {
    //        vec2 sampleOffset = TexCoords + vec2(offset.x * float(i), offset.y * float(j));
    //        pixel = texture(psTexture, sampleOffset).rgba;
    //        nColors += 1;
    //        result += pixel;
    //    }
    //}
    //if (nColors > 0) {
    //    result /= float(nColors);
    //}

    //result = texture(psTexture, TexCoords).rgba;

    FragColor = result;
}