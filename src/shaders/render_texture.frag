#version 330 core

out vec4 fragmentColor;

in vec2 textureCoordinates;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    /* float kernel[9] = float[]( */
    /*     -1, -1, -1, */
    /*     -1,  9, -1, */
    /*     -1, -1, -1 */
    /* ); */
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    vec3 samples[9];
    for(int i = 0; i < 9; ++i)
    {
        samples[i] = vec3(texture(screenTexture, textureCoordinates.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; ++i)
    {
        color += samples[i] * kernel[i];
    }
    fragmentColor = vec4(color, 1.0);
}
