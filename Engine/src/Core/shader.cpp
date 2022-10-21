#include <SDL.h>

#include "shader.h"
#include "funcutils.h"
#include "logging.h"

namespace Ossium
{

    //
    // Shader
    //

    REGISTER_RESOURCE(Shader);

    Shader::~Shader()
    {
        Free();
    }

    // Load material from file
    bool Shader::Load(std::string guid_path)
    {
        // Make sure memory is free first
        Free();

        // Open the file
        SDL_RWops* file = SDL_RWFromFile(guid_path.c_str(), "rb");
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
            handle = bgfx::createShader(buffer);
            if (!bgfx::isValid(handle))
            {
                Log.Error("Failed to create shader from file at \"{0}\"", guid_path);
            }
        }
        else
        {
            // File could not be opened
            // TODO: return in error message object
            Log.Warning("Failed to open shader at path \"{0}\". {1}", guid_path, SDL_GetError());
        }
        return bgfx::isValid(handle);
    }

    // Initialise shaders
    bool Shader::Init()
    {
        return true;
    }

    // Load and initialise the material
    bool Shader::LoadAndInit(std::string guid_path)
    {
        return Load(guid_path) && Init();
    }

    bgfx::ShaderHandle Shader::GetHandle()
    {
        return handle;
    }

    bool Shader::Free()
    {
        if (bgfx::isValid(handle))
        {
            bgfx::destroy(handle);
            handle = BGFX_INVALID_HANDLE;
            return true;
        }
        return false;
    }

    std::string Shader::GetPath(std::string id)
    {
        auto backend = bgfx::getRendererType();
        switch (backend) {
            case bgfx::RendererType::Noop:
            case bgfx::RendererType::Direct3D9:
                return "assets/Shaders/dx9/" + id + ".bin";
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12:
                return "assets/Shaders/dx11/" + id + ".bin";
            case bgfx::RendererType::Gnm:
                return "assets/Shaders/pssl/" + id + ".bin";
            case bgfx::RendererType::Metal:
                return "assets/Shaders/metal/" + id + ".bin";
            case bgfx::RendererType::OpenGL:
                return "assets/Shaders/glsl/" + id + ".bin";
            case bgfx::RendererType::OpenGLES:
                return "assets/Shaders/essl/" + id + ".bin";
            case bgfx::RendererType::Vulkan:
                return "assets/Shaders/spirv/" + id + ".bin";
            default:
                std::string error = Utilities::Format(
                    "Failed to obtain path to shader \"{0}\" for current renderer backend: {1}",
                    id,
                    bgfx::getRendererName(backend)
                );
                Log.Error(error);
                throw error.c_str();
        }
    }

}
