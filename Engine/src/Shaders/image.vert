$input a_position, a_texcoord0
$output uv

#include <bgfx_shader.sh>

void main()
{
    gl_Position = vec4(a_position, 1.0);
    uv = a_texcoord0;
}
