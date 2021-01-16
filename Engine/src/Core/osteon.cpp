#include "osteon.h"
#include "resourcecontroller.h"

namespace Ossium
{

    REGISTER_COMPONENT(Osteon);

    void Osteon::OnLoadFinish()
    {
#ifdef OSSIUM_EDITOR
        if (ReloadOsteon)
        {
            ReloadOsteon = false;
            
            // First destroy the existing children
            entity->GetScene()->WalkEntities([this] (Entity* obj) {
                if (obj == entity)
                {
                    return true;
                }
                obj->Destroy(true);
                return false;
            }, true, entity);

            // Reload the scene file.
            Scene loaded = Scene(
                entity->GetScene()->GetServices()
            );
            loaded.Load(path);

            // Now move the entities from that scene to this scene.
            for (Entity* obj : loaded.GetRootEntities())
            {
                obj->SetScene(entity->GetScene(), entity);
            }
        }
#endif // OSSIUM_EDITOR
    }
    
}
