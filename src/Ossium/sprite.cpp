#include <algorithm>

#include "sprite.h"
#include "csvdata.h"

namespace Ossium
{

    inline namespace animator
    {

        ///
        /// SpriteAnimation
        ///

        REGISTER_ANIMATION(SpriteAnimation);

        bool SpriteAnimation::Load(string path)
        {
            CSV csv;
            if (csv.Import(path, false))
            {
                name = splitPair(csv.GetCell(0, 0), '=');
                spriteSheetPath = splitPair(csv.GetCell(0, 1), '=');
                loop = splitPair(csv.GetCell(0, 2), '=') == "true" ? true : false;
                duration = (Uint32)ToInt(splitPair(csv.GetCell(0, 3), '='));
                if (spriteSheetPath == "" || spriteSheetPath == "UNKNOWN")
                {
                    SDL_Log("Error getting sprite sheet path from sprite animation file '%s'.", path.c_str());
                    return false;
                }
                for (unsigned int i = 2, counti = csv.data.empty() ? 0 : csv.data.size(); i < counti; i++)
                {
                    SpriteKeyframe keyframe;
                    keyframe.timePosition = (Uint32)ToInt(csv.GetCell(i, 0));
                    keyframe.clipArea.x = (Uint32)ToInt(csv.GetCell(i, 1));
                    keyframe.clipArea.y = (Uint32)ToInt(csv.GetCell(i, 2));
                    keyframe.clipArea.w = (Uint32)ToInt(csv.GetCell(i, 3));
                    keyframe.clipArea.h = (Uint32)ToInt(csv.GetCell(i, 4));
                    keyframe.position.x = ToFloat(csv.GetCell(i, 5));
                    keyframe.position.y = ToFloat(csv.GetCell(i, 6));
                    keyframe.width = ToFloat(csv.GetCell(i, 7));
                    keyframe.height = ToFloat(csv.GetCell(i, 8));
                    keyframe.origin.x = ToFloat(csv.GetCell(i, 9));
                    keyframe.origin.y = ToFloat(csv.GetCell(i, 10));
                    keyframe.direction.x = ToFloat(csv.GetCell(i, 11));
                    keyframe.direction.y = ToFloat(csv.GetCell(i, 12));
                    keyframes.insert(keyframe);
                }
                return Image::Load(spriteSheetPath);
            }
            return false;
        }

        bool SpriteAnimation::LoadAndInit(string path, Renderer& renderer, Uint32 windowPixelFormat, bool cache)
        {
            return Load(path) && Image::Init(renderer, windowPixelFormat, cache);
        }

        void SpriteAnimation::Export(string path)
        {
            Export(path, "");
        }

        void SpriteAnimation::Export(string path, string spriteSheet)
        {
            CSV csv;
            spriteSheet = spriteSheet.length() == 0 ? (spriteSheetPath.length() == 0 ? "UNKNOWN" : spriteSheetPath) : spriteSheet;
            csv.data.push_back({
                "name=" + name,
                "spritesheet=" + spriteSheet,
                "loopable=" + (string)(loop ? "true" : "false"),
                "duration=" + ToString((int)duration)
            });
            /// Add the headers.
            csv.data.push_back({
                "Time position (ms):",
                "Clip Area x:",
                "Clip Area y:",
                "Clip Area w:",
                "Clip Area h:",
                "Position x:",
                "Position y:",
                "Render w:",
                "Render h:",
                "Origin x:",
                "Origin y:",
                "Direction x:",
                "Direction y:",
                "Transition type:"
            });
            /// Now chuck in the data from all the keyframes in this animation
            for (SpriteKeyframe keyframe : keyframes)
            {
                csv.data.push_back({
                    ToString((int)keyframe.timePosition),
                    ToString(keyframe.clipArea.x),
                    ToString(keyframe.clipArea.y),
                    ToString(keyframe.clipArea.w),
                    ToString(keyframe.clipArea.h),
                    ToString(keyframe.position.x),
                    ToString(keyframe.position.y),
                    ToString(keyframe.width),
                    ToString(keyframe.height),
                    ToString(keyframe.origin.x),
                    ToString(keyframe.origin.y),
                    ToString(keyframe.direction.x),
                    ToString(keyframe.direction.y)
                });
            }
            csv.Export(path, false);
        }

        SpriteKeyframe SpriteAnimation::GetSample(AnimatorClip* clip)
        {
            if (!keyframes.empty())
            {
                /// Find the closest keyframe with lower_bound
                defaultKeyframe.timePosition = clip->GetTime();
                auto sample = --lower_bound(keyframes.begin(), keyframes.end(), defaultKeyframe);
                defaultKeyframe.timePosition = 0;
                if (sample == keyframes.end())
                {
                    /// Animation has looped round.
                    return *keyframes.begin();
                }
                return *sample;
            }
            return defaultKeyframe;
        }

    }

    ///
    /// Sprite
    ///

    REGISTER_COMPONENT(Sprite);

    void Sprite::PlayAnimation(AnimatorTimeline& timeline, SpriteAnimation* animation, int startTime, int loops, bool autoRemove)
    {
        anim.SetAnimation(animation);
        SetSource(static_cast<Image*>(animation));
        anim.SetLoops(loops);
        anim.Play(timeline, startTime, autoRemove);
    }

    /// TODO: all the positioning/movement stuff
    void Sprite::Update()
    {
        SpriteKeyframe kf = anim.Sample<SpriteKeyframe>();
        SetClip(kf.clipArea.x, kf.clipArea.y, kf.clipArea.w, kf.clipArea.h);
    }

}
