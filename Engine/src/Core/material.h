#ifndef MATERIAL_H
#define MATERIAL_H

#include "resourcecontroller.h"
#include "shader.h"
#include "bgfx/bgfx.h"

namespace Ossium
{

    // A material consists of one or more shaders and their input/attribute values,
    // with the exception of mesh-specific data such as vertices (typically).
    class Material : public Resource
    {
    public:
        DECLARE_RESOURCE(Material);

        // Load from a .MTL file.
        bool Load(std::string guid_path);

        // Initialise relevant shader.
        bool Init(ResourceController* resources);

        // Load from a .MTL file, then initialise the relevant shader.
        bool LoadAndInit(std::string guid_path, ResourceController* resources);

        // Get the shader associated with this material.
        Shader* GetShader();

    private:
        // The shaders associated with this material.
        Shader* shaderVertex = nullptr;
        Shader* shaderFragment = nullptr;

        std::string shaderVertexPath = "standard.vert";
        std::string shaderFragmentPath = "standard.frag";
    };
    
}

#endif // MATERIAL_H
