#include "LayoutComponent.h"


namespace Ossium
{

    struct BoxLayoutSchema : public Schema<BoxLayoutSchema, 20>
    {
        DECLARE_BASE_SCHEMA(BoxLayoutSchema, 20);
        
        // The top-left of the box, relative percent to the chain of parent box layouts.
        // If there is no parent box layout, acts relative to the renderer dimensions.
        M(Vector2, anchorMin) = Vector2::Zero;
        // The bottom-right of the box, relative percent to the chain of parent box layouts.
        // If there is no parent box layout, acts relative to the renderer dimensions.
        M(Vector2, anchorMax) = Vector2::OneOne;

        // The origin determines where the Transform component is positioned relative to anchorMin and anchorMax
        // where (1, 1) is anchorMax, (0, 0) is anchorMin, and (0.5, 0.5) is half way between (the centre).
        M(Vector2, origin) = {0.5f, 0.5f};

        // The relative percent amount of space between anchorMax and the centre of the box.
        // (0, 0) is no padding, (1, 1) is full padding.
        M(Vector2, paddingMin) = Vector2::Zero;
        // The relative percent amount of space between anchorMax and the centre of the box.
        // (0, 0) is no padding, (1, 1) is full padding.
        M(Vector2, paddingMax) = Vector2::Zero;

    };


    class BoxLayout : public LayoutComponent, public BoxLayoutSchema
    {
    public:
        CONSTRUCT_SCHEMA(LayoutComponent, BoxLayoutSchema);
        DECLARE_COMPONENT(LayoutComponent, BoxLayout);
        
        void OnLoadFinish();

        void LayoutRefresh();

        // Returns the combined anchorMin (factors in ancestor BoxLayouts).
        Vector2 GetCumulativeAnchorMin();
        // Returns the combined anchorMax (factors in ancestor BoxLayouts).
        Vector2 GetCumulativeAnchorMax();

        // Returns the absolute (pixel) position of anchorMin.
        Vector2 GetAnchorMinPosition();
        // Returns the absolute (pixel) position of anchorMax.
        Vector2 GetAnchorMaxPosition();

        // Returns the difference between the absolute anchorMin position and the absolute anchorMax position.
        Vector2 GetDimensions();

        // Returns the difference between the absolute paddingMin position and the absolute paddingMax position.
        Vector2 GetInnerDimensions();

    private:
        // Cached percent of anchorMin.
        Vector2 anchorMinCached;
        // Cached percent of anchorMax.
        Vector2 anchorMaxCached;

    };

}
