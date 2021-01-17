#ifndef OSTEON_H
#define OSTEON_H

#include "component.h"

namespace Ossium
{
    
    struct OsteonSchema : public Schema<OsteonSchema, 20>
    {
        DECLARE_BASE_SCHEMA(OsteonSchema, 20);

        // The colour of entities that make up the Osteon in the editor entity hierarchy.
        M(SDL_Color, groupingColor) = Color(100, 0, 190, 255);

        // The path to the scene file to use.
        SCHEMA_MEMBER(ATTRIBUTE_FILEPATH, std::string, path) = "assets/Osteons/";

        // Toggling this from false to true in the editor will update everything (reload the scene file).
        // Beware as this will erase and overwrite all inline changes.
        M(bool, ReloadOsteon) = true;

    };
    
    /// Osteons are special components that can load scenes inline.
    /// Note, an Osteon controls all of it's children
    /// such that if an entity in the scene file is added, removed or modified,
    /// when the Osteon is manually updated it will overwrite all children with the scene file.
    /// This means an Osteon will destroy any children that are not in the scene file
    /// and erase any inline changes made to children beneath it when reloaded, so beware.
    /// TODO: custom editor buttons to reload, apply changes and so on.
    class Osteon : public Component, public OsteonSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, OsteonSchema);
        DECLARE_COMPONENT(Component, Osteon);
        
        void OnLoadFinish();

        void Reload();
        
    };
    
}

#endif // OSTEON_H
