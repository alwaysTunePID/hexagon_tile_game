uniform sampler2D texturee;
uniform sampler2D reflection;
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

    // Properties
    const int octaves = 5;
    float lacunarity = 2.0;
    float gain = 0.5;
    //
    // Initial values
    float amplitude_y = 0.0005 * ZOOM;
    float amplitude_x = 0.0001 * ZOOM;
    float frequency = 20.0 / ZOOM; // 1.0;
    //
    float x = (x_origin + gl_TexCoord[0].x * TILE_SURFACE_WITDH) / (TILE_SURFACE_WITDH / 30 ) ;
    float y = (y_origin + gl_TexCoord[0].y * TILE_SURFACE_HEIGHT) / (TILE_SURFACE_HEIGHT / 30 ) ;
    float x_w = gl_TexCoord[0].x;
    float y_w = gl_TexCoord[0].y;
    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        x_w += amplitude_x * sin(frequency * y + timee);
        y_w += amplitude_y * sin(frequency * x + timee);
        frequency *= lacunarity;
        amplitude_x *= gain;
        amplitude_y *= gain;
    }

    vec4 pixel = texture2D(texturee, vec2(x_w, y_w));  // y
    vec4 pixel_ref = texture2D(reflection, vec2(x_w * TILE_SURFACE_WITDH / REFLECTION_WIDTH , 1.0 - y_w *  TILE_SURFACE_HEIGHT / REFLECTION_HEIGHT));
    pixel.a *= 0.8;
    WATER_COLOR.a *= 0.8;

    gl_FragColor = pixel * pixel_ref; //  gl_Color * (pixel / 16.0);
}
