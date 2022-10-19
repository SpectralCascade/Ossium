$input a_position, a_color0
$output color

#include <bgfx_shader.sh>

void main()
{
    gl_Position = vec4(a_position, 1.0);
    color = a_color0;
}
