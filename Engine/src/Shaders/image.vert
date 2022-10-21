$input a_position, a_texcoord0, a_color0
$output uv, color

#include <bgfx_shader.sh>

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position.x, a_position.y, 0.0, 1.0));
    uv = a_texcoord0;
    color = a_color0;
}
