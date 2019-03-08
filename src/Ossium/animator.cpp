#include "animator.h"

using namespace std;

namespace Ossium
{

    inline namespace animator
    {

        ///
        /// BaseAnimation
        ///

        BaseAnimation::~BaseAnimation()
        {
        }

        bool BaseAnimation::IsLoopable()
        {
            return loop;
        }

        Uint32 BaseAnimation::GetDuration()
        {
            return duration;
        }

        string BaseAnimation::GetName()
        {
            return name;
        }

        bool BaseAnimation::CanTween()
        {
            return tween;
        }

        bool operator<(const BaseKeyframe& lhs, const BaseKeyframe& rhs)
        {
            return lhs.timePosition < rhs.timePosition;
        }

        ///
        /// AnimatorClip
        ///

        void AnimatorClip::SetAnimation(BaseAnimation* animation)
        {
            anim = animation;
            /// -1 because wrap has an inclusive min and max.
            localClock.SetWrap(anim->GetDuration() - 1);
        }

        Uint32 AnimatorClip::GetStartTime()
        {
            return globalStartTime;
        }

        void AnimatorClip::SetLoops(int n)
        {
            loops = n;
        }

        int AnimatorClip::GetLoops()
        {
            return loops;
        }

        int AnimatorClip::GetDuration()
        {
            int duration = (int)((float)((loops + 1) * (int)anim->GetDuration()) * localClock.GetScaleFactor());
            return (loops < 0 ? -1 : (duration < 0 ? -duration : duration));
        }

        Uint32 AnimatorClip::GetSourceDuration()
        {
            return anim->GetDuration();
        }

        bool AnimatorClip::Update(Clock& globalClock)
        {
            if (globalClock.GetDeltaTime() < 0)
            {
                if (globalClock.GetTime() < globalStartTime)
                {
                    timesLooped = 0;
                    if (!localClock.IsPaused() && localClock.GetScaleFactor() != 0.0f)
                    {
                        /// Quantize to beginning
                        localClock.SetTime(0);
                    }
                }
                else
                {
                    if (localClock.Update(globalClock.GetDeltaTime()) && timesLooped > 0)
                    {
                        timesLooped--;
                    }
                }
            }
            else if (globalClock.GetTime() > globalStartTime)
            {
                if (localClock.Update(globalClock.GetDeltaTime()))
                {
                    if (loops < 0 || timesLooped < loops)
                    {
                        timesLooped++;
                    }
                    else
                    {
                        /// Quantize to end
                        localClock.SetTime(GetSourceDuration());
                        return true;
                    }
                }
            }
            return false;
        }

        void AnimatorClip::Play(AnimatorTimeline& timeline, int startTime, bool removeOnFinish)
        {
            if (IsPlaying())
            {
                Stop();
            }
            playingTimeline = &timeline;
            Reset();
            timeline.Play(this, startTime, removeOnFinish);
        }

        void AnimatorClip::Pause()
        {
            localClock.SetPaused(true);
        }

        void AnimatorClip::Resume()
        {
            localClock.SetPaused(false);
        }

        bool AnimatorClip::IsPaused()
        {
            return localClock.IsPaused();
        }

        bool AnimatorClip::IsPlaying()
        {
            return playingTimeline != nullptr;
        }

        void AnimatorClip::Stop()
        {
            if (playingTimeline != nullptr)
            {
                playingTimeline->Stop(this);
                playingTimeline = nullptr;
            }
        }

        void AnimatorClip::Reset()
        {
            timesLooped = 0;
            localClock.SetTime(0);
        }

        void AnimatorClip::SetStretch(float sf)
        {
            localClock.Stretch(sf);
        }

        float AnimatorClip::GetStretch()
        {
            return localClock.GetScaleFactor();
        }

        Uint32 AnimatorClip::GetTime()
        {
            return localClock.GetTime();
        }

        AnimatorTimeline* AnimatorClip::GetTimeline()
        {
            return playingTimeline;
        }

        ///
        /// AnimatorTimeline
        ///

        AnimatorTimeline::~AnimatorTimeline()
        {
            curves.clear();
            ClearClips();
        }

        void AnimatorTimeline::Update(float deltaTime)
        {
            /// Update the global and clip timelines.
            clock.Update(deltaTime);
            if (clock.GetScaleFactor() < 0.0f)
            {
                /// Reverse direction; the beginning of the clip is treated as the end of the clip!
                for (auto itr = clips.end() - 1; itr != clips.begin() - 1; itr--)
                {
                    if (clock.GetTime() >= (*itr)->GetStartTime())
                    {
                        if ((*itr)->Update(clock) && (*itr)->autoRemove)
                        {
                            /// Clip has finished playing; remove it from the global timeline.
                            itr = clips.erase(itr);
                        }
                    }
                    else
                    {
                        /// Early out; clips that aren't yet playing don't need to be updated.
                        break;
                    }
                }
            }
            else
            {
                for (auto itr = clips.begin(); itr != clips.end(); itr++)
                {
                    if ((*itr)->GetStartTime() < clock.GetTime())
                    {
                        if ((*itr)->Update(clock) && (*itr)->autoRemove)
                        {
                            /// Clip has finished playing; remove it from the global timeline.
                            itr = --clips.erase(itr);
                        }
                    }
                    else
                    {
                        /// Early out; clips that aren't yet playing don't need to be updated.
                        break;
                    }
                }
            }
        }

        void AnimatorTimeline::Play(AnimatorClip* clip, int startTime, bool removeOnFinish)
        {
            clip->globalStartTime = startTime + (int)clock.GetTime() < 0 ? 0 : startTime + (int)clock.GetTime();
            clip->autoRemove = removeOnFinish;
            /// This could be improved using a set with a comparator which returns lhs.globalStartTime < rhs.globalStartTime
            /// Note: has not been done because GCC 8 std::set predicates might be bugged (with pointers at least).
            for (auto i = clips.begin(); i != clips.end(); i++)
            {
                if (clip->globalStartTime < (*i)->globalStartTime)
                {
                    clips.insert(i, clip);
                    return;
                }
            }
            clips.push_back(clip);
        }

        void AnimatorTimeline::Stop(AnimatorClip* clip)
        {
            for (auto i = clips.begin(); i != clips.end(); i++)
            {
                if (clip == (*i))
                {
                    clips.erase(i);
                    return;
                }
            }
        }

        void AnimatorTimeline::Play(AnimatorClip* previous, AnimatorClip* next, float transitionTime)
        {
        }

        Uint8 AnimatorTimeline::AddTweeningFuncs(vector<CurveFunction> easingCurves)
        {
            unsigned int index = curves.size();
            for (auto i : easingCurves)
            {
                if (curves.size() < 256)
                {
                    curves.push_back(i);
                    continue;
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Unable to add more tweening functions to animator timeline. Number of functions exceeds the 255 function limit!");
                    return (Uint8)(index - 1);
                }
            }
            return (Uint8)index;
        }

        CurveFunction AnimatorTimeline::GetTweeningFunc(Uint8 index)
        {
            return (index >= (Uint8)curves.size() ? nullptr : curves[index]);
        }

        void AnimatorTimeline::ClearTweeningFuncs()
        {
            curves.clear();
            curves.push_back(nullptr);
        }

        void AnimatorTimeline::ClearClips()
        {
            clips.clear();
        }

    }

}
