#include <SDL.h>

#include "shader.h"
#include "funcutils.h"
#include "logging.h"

namespace Ossium
{

    std::string GetShaderPath(std::string file)
    {
        auto backend = bgfx::getRendererType();
        switch (backend) {
            case bgfx::RendererType::Noop:
            case bgfx::RendererType::Direct3D9:
                return "shaders/dx9/";
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12:
                return "shaders/dx11/";
            case bgfx::RendererType::Gnm:
                return "shaders/pssl/";
            case bgfx::RendererType::Metal:
                return "shaders/metal/";
            case bgfx::RendererType::OpenGL:
                return "shaders/glsl/";
            case bgfx::RendererType::OpenGLES:
                return "shaders/essl/";
            case bgfx::RendererType::Vulkan:
                return "shaders/spirv/";
            default:
                std::string error = Utilities::Format(
                    "Failed to obtain path to shader \"{0}\" for current renderer backend: {1}",
                    file,
                    bgfx::getRendererName(backend)
                );
                Log.Error(error);
                throw error.c_str();
        }
    }
    
    bgfx::ShaderHandle LoadShader(std::string path)
    {
        bgfx::ShaderHandle shader = BGFX_INVALID_HANDLE;

        // Open the file
        SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
        Sint64 fsize = -1;
        if (file != NULL && (fsize = SDL_RWsize(file)) >= 0)
        {
            // Read the data
            const bgfx::Memory* buffer = bgfx::alloc(fsize + 1);
            size_t num_bytes = SDL_RWread(file, buffer->data, sizeof(uint8_t), fsize);
            if (num_bytes > 0)
            {
                // Null terminate
                buffer->data[num_bytes] = '\0';
            }
            SDL_RWclose(file);
            
            // Create shader from data
            shader = bgfx::createShader(buffer);
        }
        else
        {
            // File could not be opened
            // TODO: return in error message object
            Log.Warning("Failed to open shader at path \"{0}\". {1}", path, SDL_GetError());
        }

        return shader;
    }

}
