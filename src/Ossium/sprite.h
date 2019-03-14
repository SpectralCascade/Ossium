#ifndef SPRITE_H
#define SPRITE_H

#include "texture.h"
#include "animator.h"

namespace Ossium
{

    inline namespace animator
    {

        class SpriteKeyframe : public BaseKeyframe, public MetaRect
        {
        public:
            /// The area of the image to be rendered in this keyframe.
            SDL_Rect clipArea = {0, 0, 0, 0};

            /// Array of keyframe transition data. Defaults to no interpolation.
            Uint8 transitions[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        };

        class SpriteAnimation : public Animation<SpriteKeyframe>, public Image
        {
        public:
            DECLARE_ANIMATION(SpriteAnimation);

            /// Loads a sprite animation, which consists of a CSV file containing keyframe data and a separate sprite sheet image.
            bool Load(string path);

            /// Default export method.
            void Export(string path);

            /// Exports a sprite animation. Does not save the image, but does store the relative path.
            void Export(string path, string spriteSheet);

            /// Returns a sample of the animation at the current local time of the given clip.
            SpriteKeyframe GetSample(AnimatorClip* clip);

            /// Overrides Image method of the same name.
            bool LoadAndInit(string path, Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_UNKNOWN, bool cache = false);

        private:
            using Image::CreateFromText;

            /// The file path to the sprite sheet for this animation.
            string spriteSheetPath;

        };

    }

    class Sprite : public Texture
    {
    public:
        DECLARE_COMPONENT(Sprite);

        /// Starts playing an animation (sets the clip sprite sheet image to the animation).
        void PlayAnimation(AnimatorTimeline& timeline, SpriteAnimation* animation, int startTime = 0, int loops = -1, bool autoRemove = true);

        /// Updates this sprite. Note this is automatically called by the ECS.
        void Update();

        /// Overrides Texture to account for offsets.
        void SetRenderWidth(float percent);
        /// Overrides Texture to account for offsets.
        void SetRenderHeight(float percent);

        /// Overrides Texture to account for percent width and percent height.
        void SetClip(int x, int y, int w = 0, int h = 0, bool autoscale = true);

        /// Current animator clip associated with this sprite.
        AnimatorClip anim;

    protected:
        /// Whether or not this sprite has ever had PlayAnimation() called or not. Necessary for configuring dimensions the first time an animation is loaded.
        bool initialised = false;

        /// Stores the temporary position offset so the position can be reverted to normal when no longer animating.
        Point positionOffset = {0, 0};

        /// Used in a very similar way to positionOffset.
        Point originOffset = {0, 0};

        /// Stores the temporary width of the sprite as percentage, relative to the actual render width
        float percentWidth = 1;
        /// Stores the temporary height of the sprite as percentage, relative to the actual render height
        float percentHeight = 1;

        /// The angular offset of the sprite, used in a similar way to positionOffset
        float angleOffset = 0;

    };

}

#endif // SPRITE_H
