// Take UV coordinates and colour modulation factor
$input uv, color

// Include necessary stuff
#include <bgfx_shader.sh>

// Sample from texture on the GPU
SAMPLER2D(tex0, 0);

void main()
{
    // Set output colour of this fragment by sampling the texture
    gl_FragColor = mul(color, texture2D(tex0, uv));
}
