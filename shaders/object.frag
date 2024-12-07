uniform sampler2D texturee;
uniform sampler2D normalMap;
uniform vec3 negScreenLightDir;
uniform float x_origin;
uniform float WIDTH2;
uniform float TILE_WIDTH;

void main()
{
    vec4 pixel = texture2D(texturee, gl_TexCoord[0].xy);

    vec4 tangentNormal = texture2D(normalMap, gl_TexCoord[0].xy) * 2.0 - 1.0;
    //vec3 tangentNormal = vec3(0.0, 1.0, 0.0);
    float grayScale = (dot(tangentNormal.xyz, negScreenLightDir) + 1.0) * 0.5;  //tangentNormal.xyz

    gl_FragColor = pixel * vec4(grayScale, grayScale, grayScale, 1.0); // vec4(grayScale, grayScale, grayScale, pixel.a); 
}
