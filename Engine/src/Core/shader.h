#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "bgfx/bgfx.h"

namespace Ossium
{

    // Get the correct shader path according to the current rendering backend
    std::string GetShaderPath(std::string file);

    // Load a shader from the renderer-specific file
    bgfx::ShaderHandle LoadShader(std::string path);

}

#endif // SHADER_H
