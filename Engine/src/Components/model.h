#ifndef MODEL_H
#define MODEL_H

#include "../Core/component.h"

namespace Ossium
{
    
    struct ModelSchema : public Schema<ModelSchema, 20>
    {
        DECLARE_BASE_SCHEMA(ModelSchema, 20);
        
        // Resource GUIDs for the meshes
        M(std::vector<std::string>, meshes);

    };

    // A 3D model that can be rendered, consisting of one or more meshes and materials.
    class Model : public RenderComponent, public ModelSchema
    {
    public:
        CONSTRUCT_SCHEMA(RenderComponent, ModelSchema);
        DECLARE_COMPONENT(RenderComponent, Model);
        
        // Render the model
        void Render(RenderInput* pass, const Matrix<4, 4>& view, const Matrix<4, 4>& proj);
        
    };
    
}



#endif // MODEL_H
