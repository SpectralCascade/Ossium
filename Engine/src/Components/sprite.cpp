/** COPYRIGHT NOTICE
 *  
 *  Copyright (c) 2018-2020 Tim Lane
 *  
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  
 *  3. This notice may not be removed or altered from any source distribution.
 *  
**/
#include <algorithm>

#include "sprite.h"
#include "csvdata.h"

namespace Ossium
{

    inline namespace Animator
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
                if (csv.data[0].empty() || csv.data[0].size() < 4)
                {
                    Logger::EngineLog().Info("Error getting animation data from sprite animation file '{0}'.", path);
                    return false;
                }
                name = SplitRight(csv.GetCell(0, 0), '=');
                spriteSheetPath = SplitRight(csv.GetCell(0, 1), '=');
                loop = SplitRight(csv.GetCell(0, 2), '=') == "true" ? true : false;
                duration = (Uint32)ToInt(SplitRight(csv.GetCell(0, 3), '='));
                tween = SplitRight(csv.GetCell(0, 4), '=') == "true" ? true : false;
                if (spriteSheetPath == "" || spriteSheetPath == "UNKNOWN")
                {
                    Logger::EngineLog().Warning("Failed to get sprite sheet path from sprite animation file '{0}'.", path);
                    return false;
                }
                if (tween)
                {
                    for (unsigned int i = 2, counti = csv.data.empty() ? 0 : csv.data.size(); i < counti; i++)
                    {
                        SpriteKeyframe keyframe;
                        keyframe.timePosition = (Uint32)ToInt(SplitLeft(csv.GetCell(i, 0), ':'));
                        keyframe.clipArea.x = (Uint32)ToInt(SplitLeft(csv.GetCell(i, 1), ':'));
                        keyframe.clipArea.y = (Uint32)ToInt(SplitLeft(csv.GetCell(i, 2), ':'));
                        keyframe.clipArea.w = (Uint32)ToInt(SplitLeft(csv.GetCell(i, 3), ':'));
                        keyframe.clipArea.h = (Uint32)ToInt(SplitLeft(csv.GetCell(i, 4), ':'));
                        keyframe.position.x = ToFloat(SplitLeft(csv.GetCell(i, 5), ':'));
                        keyframe.position.y = ToFloat(SplitLeft(csv.GetCell(i, 6), ':'));
                        keyframe.width = ToFloat(SplitLeft(csv.GetCell(i, 7), ':'));
                        keyframe.height = ToFloat(SplitLeft(csv.GetCell(i, 8), ':'));
                        keyframe.origin.x = ToFloat(SplitLeft(csv.GetCell(i, 9), ':'));
                        keyframe.origin.y = ToFloat(SplitLeft(csv.GetCell(i, 10), ':'));
                        keyframe.angle = ToFloat(SplitLeft(csv.GetCell(i, 11), ':'));
                        /// Now the Tweening transitions
                        keyframe.transitions[0] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 1), ':', "0"));
                        keyframe.transitions[1] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 2), ':', "0"));
                        keyframe.transitions[2] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 3), ':', "0"));
                        keyframe.transitions[3] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 4), ':', "0"));
                        keyframe.transitions[4] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 5), ':', "0"));
                        keyframe.transitions[5] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 6), ':', "0"));
                        keyframe.transitions[6] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 7), ':', "0"));
                        keyframe.transitions[7] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 8), ':', "0"));
                        keyframe.transitions[8] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 9), ':', "0"));
                        keyframe.transitions[9] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 10), ':', "0"));
                        keyframe.transitions[10] = (Uint8)ToInt(SplitRight(csv.GetCell(i, 11), ':', "0"));
                        /// Insert the loaded keyframe
                        keyframes.insert(keyframe);
                    }
                }
                else
                {
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
                        keyframe.angle = ToFloat(csv.GetCell(i, 11));
                        /// Insert the loaded keyframe
                        keyframes.insert(keyframe);
                    }
                }
                return Image::Load(spriteSheetPath);
            }
            return false;
        }

        bool SpriteAnimation::LoadAndInit(string path, Renderer& renderer, Uint32 pixelFormatting, bool cache)
        {
            return Load(path) && Image::Init(renderer, pixelFormatting, cache);
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
                "duration=" + ToString((int)duration),
                "Tweening=" + (string)(tween ? "true" : "false")
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
                "Angle:"
            });
            /// Now chuck in the data from all the keyframes in this animation
            if (tween)
            {
                for (SpriteKeyframe keyframe : keyframes)
                {
                    csv.data.push_back({
                        ToString((int)keyframe.timePosition),
                        ToString(keyframe.clipArea.x) + ":" + ToString((int)keyframe.transitions[0]),
                        ToString(keyframe.clipArea.y) + ":" + ToString((int)keyframe.transitions[1]),
                        ToString(keyframe.clipArea.w) + ":" + ToString((int)keyframe.transitions[2]),
                        ToString(keyframe.clipArea.h) + ":" + ToString((int)keyframe.transitions[3]),
                        ToString(keyframe.position.x) + ":" + ToString((int)keyframe.transitions[4]),
                        ToString(keyframe.position.y) + ":" + ToString((int)keyframe.transitions[5]),
                        ToString(keyframe.width) + ":" + ToString((int)keyframe.transitions[6]),
                        ToString(keyframe.height) + ":" + ToString((int)keyframe.transitions[7]),
                        ToString(keyframe.origin.x) + ":" + ToString((int)keyframe.transitions[8]),
                        ToString(keyframe.origin.y) + ":" + ToString((int)keyframe.transitions[9]),
                        ToString(keyframe.angle) + ":" + ToString((int)keyframe.transitions[10])
                    });
                }
            }
            else
            {
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
                        ToString(keyframe.angle)
                    });
                }
            }
            csv.Export(path, false);
        }

        SpriteKeyframe SpriteAnimation::GetSample(AnimatorClip* clip)
        {
            AnimatorTimeline* tl = clip->GetTimeline();
            if (!keyframes.empty() && tl != nullptr)
            {
                /// Find the closest keyframe with lower_bound
                SpriteKeyframe interpolated;
                interpolated.timePosition = clip->GetTime();
                auto sample = --lower_bound(keyframes.begin(), keyframes.end(), interpolated);
                if (sample == keyframes.end())
                {
                    /// Animation sample time is less than the first keyframe time, so return the first keyframe.
                    return *keyframes.begin();
                }
                interpolated = *sample;

                if (tween)
                {
                    /// Attempt to interpolate between the sample keyframe and the next keyframe
                    sample++;
                    float percent;
                    if (sample == keyframes.end())
                    {
                        sample = keyframes.begin();
                        percent = (float)(clip->GetTime() - interpolated.timePosition) / (float)((duration - interpolated.timePosition) + sample->timePosition);
                    }
                    else
                    {
                        percent = (float)(clip->GetTime() - interpolated.timePosition) / (float)(sample->timePosition - interpolated.timePosition);
                    }

                    /// Attempt to interpolate each member
                    interpolated.clipArea.x = AttemptTween((float)interpolated.clipArea.x, (float)sample->clipArea.x, percent, tl->GetTweeningFunc(interpolated.transitions[0]));
                    interpolated.clipArea.y = AttemptTween((float)interpolated.clipArea.y, (float)sample->clipArea.y, percent, tl->GetTweeningFunc(interpolated.transitions[1]));
                    interpolated.clipArea.w = AttemptTween((float)interpolated.clipArea.w, (float)sample->clipArea.w, percent, tl->GetTweeningFunc(interpolated.transitions[2]));
                    interpolated.clipArea.h = AttemptTween((float)interpolated.clipArea.h, (float)sample->clipArea.h, percent, tl->GetTweeningFunc(interpolated.transitions[3]));
                    interpolated.position.x = AttemptTween(interpolated.position.x, sample->position.x, percent, tl->GetTweeningFunc(interpolated.transitions[4]));
                    interpolated.position.y = AttemptTween(interpolated.position.y, sample->position.y, percent, tl->GetTweeningFunc(interpolated.transitions[5]));
                    interpolated.width = AttemptTween(interpolated.width, sample->width, percent, tl->GetTweeningFunc(interpolated.transitions[6]));
                    interpolated.height = AttemptTween(interpolated.height, sample->height, percent, tl->GetTweeningFunc(interpolated.transitions[7]));
                    interpolated.origin.x = AttemptTween(interpolated.origin.x, sample->origin.x, percent, tl->GetTweeningFunc(interpolated.transitions[8]));
                    interpolated.origin.y = AttemptTween(interpolated.origin.y, sample->origin.y, percent, tl->GetTweeningFunc(interpolated.transitions[9]));
                    interpolated.angle = AttemptTween(interpolated.angle, sample->angle, percent, tl->GetTweeningFunc(interpolated.transitions[10]));
                }

                return interpolated;
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
        animation->SetDefaultKeyframeToFirstKeyframe();
        anim.SetAnimation(animation);
        SetSource(static_cast<Image*>(animation), !initialised);
        SpriteKeyframe kf = anim.Sample<SpriteKeyframe>();
        SetClip(0, 0, kf.clipArea.w, kf.clipArea.h, !initialised);
        WorldRotation().SetDegrees(WorldRotation().GetDegrees() - angleOffset);
        WorldPosition() -= positionOffset;
        width = initialised ? (percentWidth == 0 ? (0) : (width / percentWidth)) : width * percentWidth;
        height = initialised ? (percentHeight == 0 ? (0) : (height / percentHeight)) : height * percentHeight;
        positionOffset = Point(0, 0);
        originOffset = Point(0, 0);
        angleOffset = 0;
        percentWidth = 1;
        percentHeight = 1;
        anim.SetLoops(loops);
        anim.Play(timeline, startTime, autoRemove);
        initialised = true;
    }

    void Sprite::Update()
    {
        if (anim.IsPlaying())
        {
            SpriteKeyframe kf = anim.Sample<SpriteKeyframe>();
            clip.x = kf.clipArea.x;
            clip.y = kf.clipArea.y;
            /// Note: these clip dimensions may have to be sequenced differently to correctly calculate the dimension offsets
            clip.w = kf.clipArea.w;
            clip.h = kf.clipArea.h;
            /// Offset position
            WorldPosition() -= positionOffset;
            positionOffset = kf.position;
            WorldPosition() += positionOffset;
            /// Offset origin
            origin -= originOffset;
            originOffset = kf.origin;
            origin += originOffset;
            /// Relative width and height
            width = width / percentWidth;
            percentWidth = kf.width;
            width = width * percentWidth;
            height = height / percentHeight;
            percentHeight = kf.height;
            height = height * percentHeight;
            /// Offset angle
            WorldRotation().SetDegrees(WorldRotation().GetDegrees() - angleOffset);
            angleOffset = kf.angle;
            WorldRotation().SetDegrees(WorldRotation().GetDegrees() + angleOffset);
        }
    }

    void Sprite::SetRenderWidth(float percent)
    {
        width = percentWidth == 0 ? 0 : (width / percentWidth);
        Texture::SetRenderWidth(percent * percentWidth);
    }

    void Sprite::SetRenderHeight(float percent)
    {
        height = percentHeight == 0 ? 0 : (height / percentHeight);
        Texture::SetRenderHeight(percent * percentHeight);
    }

    void Sprite::SetClip(int x, int y, int w, int h, bool autoScale)
    {
        if (autoScale)
        {
            /// Cache percentage width and height
            float wpercent = (clip.w == 0 ? 0 : width / (float)clip.w) * percentWidth;
            float hpercent = (clip.h == 0 ? 0 : height / (float)clip.h) * percentHeight;
            clip = {x, y, w, h};
            /// Recalculate destination dimensions with new clip rect
            Texture::SetRenderWidth(wpercent);
            Texture::SetRenderHeight(hpercent);
        }
        else
        {
            clip = {x, y, w, h};
        }
    }

}
