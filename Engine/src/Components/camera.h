#ifndef CAMERA_H
#define CAMERA_H

#include "../Core/component.h"

namespace Ossium
{
    
    struct CameraSchema : public Schema<CameraSchema, 20>
    {
        DECLARE_BASE_SCHEMA(CameraSchema, 20);
        
        // Field of view, in degrees
        M(float, fov) = 60.0f;

        // Minimum of the clip volume
        M(float, near) = 0;

        // Maximum of the clip volume
        M(float, far) = 100;

    };
    
    
    class Camera : public RenderInput, public Component, public CameraSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, CameraSchema);
        DECLARE_COMPONENT(Component, Camera);
        
        // Add RenderInput to main renderer
        void OnCreate();

        // Remove RenderInput from main renderer
        void OnDestroy();

        // Returns a name for graphics debugging purposes
        std::string GetRenderDebugName();
        
        // Camera always sorts primarily by depth
        bgfx::ViewMode::Enum GetViewMode();

        // Returns the camera view "LookAt" matrix
        Matrix<4, 4> GetViewMatrix();

        // Returns the camera projection matrix
        Matrix<4, 4> GetProjMatrix();

        // Render all child graphics
        void Render();
        
    };
    
}


#endif // CAMERA_H
