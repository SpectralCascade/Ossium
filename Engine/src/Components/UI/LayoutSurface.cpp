#include "LayoutSurface.h"
#include "LayoutComponent.h"

using namespace std;

namespace Ossium
{

    REGISTER_COMPONENT(LayoutSurface);

    void LayoutSurface::OnLoadFinish()
    {
        SetDirty();
        LayoutUpdate();
    }

    void LayoutSurface::LayoutUpdate()
    {
        if (dirty)
        {
            // Walk layout components breadth-first from this entity
            entity->GetScene()->WalkEntities([&] (Entity* child) {
                if (!child->IsActive())
                {
                    // Skip inactive entities
                    return false;
                }
                LayoutSurface* group = child->GetComponent<LayoutSurface>();
                if (group != nullptr && !group->IsDirty())
                {
                    // If another LayoutSurface is encountered which is not dirty, skip it.
                    return false;
                }
                vector<LayoutComponent*> layouts = child->GetComponents<LayoutComponent>();
                for (auto layout : layouts)
                {
                    if (layout->enabled)
                    {
                        // Only refresh enabled layout components
                        layout->LayoutRefresh();
                    }
                }
                return true;
            }, true, entity);
            // Once layouts are updated, this can be safely unmarked dirty.
            dirty = false;
        }
    }

    bool LayoutSurface::IsDirty()
    {
        return dirty;
    }

    void LayoutSurface::SetDirty()
    {
        dirty = true;
    }

    void LayoutSurface::OnDestroy()
    {
#ifdef OSSIUM_EDITOR
        entity->GetScene()->WalkEntities([&] (Entity* target) {
            if (target->WillBeDestroyed())
            {
                // Early out
                return false;
            }

            LayoutSurface* layout = target->GetComponent<LayoutSurface>();
            if (layout != nullptr && layout != this)
            {
                // Sub-layouts manage their own LayoutComponents.
                return false;
            }
            vector<LayoutComponent*> layoutObjs = target->GetComponents<LayoutComponent>();
            for (auto component : layoutObjs)
            {
                component->layoutSurface = entity->GetAncestor<LayoutSurface>();
                if (!component->layoutSurface)
                {
                    // Force add it if the target entity is not being destroyed, all layout components must have a LayoutSurface.
                    Log.Warning("LayoutComponent has no ancestor LayoutSurface, attempting to add replacement.");
                    component->layoutSurface = target->AddComponentOnce<LayoutSurface>();
                }
            }
            return true;
        }, true, entity);
#endif
    }

}
