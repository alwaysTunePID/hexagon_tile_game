uniform sampler2D texturee;
uniform sampler2D reflection;
uniform vec3 negScreenLightDir;
uniform float x_origin;
uniform float y_origin;
uniform vec4 WATER_COLOR;
uniform float WIDTH2;
uniform float TILE_WIDTH;
uniform float ZOOM;
uniform float timee;

uniform float TILE_SURFACE_WITDH;
uniform float TILE_SURFACE_HEIGHT;
uniform float REFLECTION_WIDTH;
uniform float REFLECTION_HEIGHT;

void main()
{

    // WolframAlpha: plot z = sin(x) + 0.5*sin(2*x) + sin(y) + 0.5*sin(2*y)

    // Try e^(sin(x)-1) instead of sin(x)

    // Properties
    const int octaves = 20;
    float lacunarity = 1.3; // 2.0;
    float gain = 0.8; // 0.5
    float e = 2.71828;
    float reflection_factor = 0.3;
    //
    // Initial values
    float amplitude = 0.0003 * ZOOM; // 0.0005
    float frequency = 20.0 / ZOOM; // 1.0;
    //
    float x = (x_origin + gl_TexCoord[0].x * TILE_SURFACE_WITDH) / (TILE_SURFACE_WITDH / 30.0 ) ;
    float y = (y_origin + gl_TexCoord[0].y * TILE_SURFACE_HEIGHT) / (TILE_SURFACE_HEIGHT / 30.0 ) ;
    float x_w = gl_TexCoord[0].x;
    float y_w = gl_TexCoord[0].y;
    float z_w = 0.0;
    float z = 0.0;
    float z_max = 0.0;
    float dfdx = 0.0;
    float dfdy = 0.0;
    float dfdz = -1.0;

    vec2 d_arr[octaves] = vec2[octaves](
        vec2(-0.4222, 0.9065),
        vec2(-0.2951, 0.9555),
        vec2(0.6353, -0.7723),
        vec2(0.9743, 0.2251),
        vec2(0.5860, -0.8103),
        vec2(-0.8429, 0.5380),
        vec2(-0.7853, -0.6192),
        vec2(-0.5356, 0.8445),
        vec2(0.9877, 0.1562),
        vec2(-0.1509, -0.9886), //
        vec2(0.4119, -0.9112),
        vec2(0.5866, -0.8099),
        vec2(-0.8988, 0.4383),
        vec2(-0.8767, -0.4811),
        vec2(0.8839, -0.4677),
        vec2(-0.9913, -0.1314),
        vec2(0.2384, 0.9712),
        vec2(-0.9944, 0.1057),
        vec2(-0.9277, 0.3733),
        vec2(-0.9980, -0.0636)
    );

    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        //float dot_prod = d_arr[i].x * x + d_arr[i].y * y;
        float phi = d_arr[i].y * 3.14;
        float h = frequency * (d_arr[i].x * x + d_arr[i].y * y) + timee + phi;
        z = pow(gain, i) * pow(e, (sin(h) - 1.0));
        z_w += z;
        float dgdx = frequency * d_arr[i].x * cos(h);
        float dgdy = frequency * d_arr[i].y * cos(h);
        dfdx +=  dgdx * z;
        dfdy +=  dgdy * z;
        z_max += pow(gain, i);
        frequency *= lacunarity;
    }

    vec4 pixel = texture2D(texturee, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + z_w * amplitude));
    vec4 pixel_ref = texture2D(reflection, vec2(x_w * TILE_SURFACE_WITDH / REFLECTION_WIDTH , 1.0 - (gl_TexCoord[0].y + z_w * amplitude) *  TILE_SURFACE_HEIGHT / REFLECTION_HEIGHT));
    pixel.a *= 0.8;
    //pixel_ref.a = pixel.a;
    //WATER_COLOR.a *= 0.8;

    float z_normalized = (z_w - 0.2) / (z_max - 0.2); 
    vec4 height_color = vec4(0.0, z_normalized, 0.0, pixel.a);
    vec4 water_height_color = vec4(z_normalized, z_normalized, 1.0, pixel.a);

    // TODO: Incorrect sign for dfdz. The neg is just because of your current mishandling of coordinate system in the z position wave function.
    vec3 wave_n = vec3(dfdx, dfdy, -dfdz);
    float n_len = length(wave_n);
    wave_n = vec3(wave_n.x/n_len, wave_n.y/n_len, wave_n.z/n_len);
    vec4 wave_n_color = vec4((wave_n + 1.0) * 0.5, pixel.a);

    float light_ref_scale = clamp(dot(wave_n, negScreenLightDir), 0.0, 1.0);
    //vec4 water_and_ref = mix(pixel, pixel_ref, reflection_factor); // Tried this but got too light blue water 
    vec4 water_light_ref = mix(pixel * pixel_ref, vec4(1.0, 1.0, 1.0, pixel.a), light_ref_scale); //mix

    //vec3 normal = normalize(vec3(dfdx, dfdy, 0.005));
    float grayScale = (dot(vec3(dfdx, dfdy, -0.0005), vec3(0.4, 0.8, -0.2)) + 1.0) * 0.5;
    gl_FragColor = water_light_ref; // WATER_COLOR * vec4(grayScale, grayScale, grayScale, pixel.a);  // pixel * pixel_ref;
}
