// Take UV coordinates
$input uv

// Include necessary stuff
#include <bgfx_shader.sh>

// Sample from texture on the GPU
SAMPLER2D(tex0, 0);

void main()
{
    // Set output colour of this fragment by sampling the texture
    gl_FragColor = texture2D(tex0, uv);
}
