#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <set>
#include <SDL.h>

#include "fsm.h"
#include "time.h"
#include "renderer.h"
#include "basics.h"

using namespace std;

namespace Ossium
{

    inline namespace animator
    {

        /// Function pointer for curve/tweening functions.
        typedef float (*CurveFunction)(float normalisedTime);

        /// The animation type identifier, for animation types such as skeleton animation, sprite animation etc.
        typedef Uint32 AnimationType;

        /// Helper macros for declaring and defining types for specific animation types, such as skeletal animation or sprite animation.
        #define DECLARE_ANIMATION(TYPE) public: static Ossium::typesys::TypeRegistry<AnimationType> __anim_type_entry_
        #define REGISTER_ANIMATION(TYPE) Ossium::typesys::TypeRegistry<AnimationType> TYPE::__anim_type_entry_

        /// Constant return type id for a specified animation method.
        template<class T>
        AnimationType getAnimationType()
        {
            return T::__anim_type_entry_.getType();
        }

        /// Represents a single key frame in an animation clip (without specific data).
        class BaseKeyframe
        {
        public:
            /// The position of this keyframe on the animation clip's local timeline, in milliseconds.
            Uint32 timePosition = 0;

            /// The tweening function used to transition to the next keyframe.
            CurveFunction tweenFunction = nullptr;

            bool operator<(const BaseKeyframe& other);

        };

        bool operator<(const BaseKeyframe& lhs, const BaseKeyframe& rhs);

        /// Forward declaration
        class AnimatorClip;

        /// An abstract interface for accessing the Animation template class.
        class BaseAnimation
        {
        public:
            virtual ~BaseAnimation();

            /// Is this animation intended for looping?
            bool IsLoopable();

            /// Returns the duration of this animation in milliseconds.
            Uint32 GetDuration();

            /// Returns the name of this animation
            string GetName();

        protected:
            /// The name of this animation
            string name = "";

            /// Meta data indicating whether this animation should be looped or not.
            bool loop = false;

            /// The duration of this animation in milliseconds.
            Uint32 duration = 0;

        };

        template<class KeyframeType>
        class Animation : public BaseAnimation
        {
        public:
            virtual ~Animation()
            {
            }

            /// Loads an animation.
            virtual bool Load(string path) = 0;

            /// Exports the animation.
            virtual void Export(string path) = 0;

            /// Samples the animation at the current local time of the provided animator clip.
            virtual KeyframeType GetSample(AnimatorClip* clip) = 0;

            /// Adds a single keyframe.
            void AddKeyframe(KeyframeType keyframe)
            {
                keyframes.insert(keyframe);
            }

            /// Removes a single keyframe.
            void RemoveKeyframe(KeyframeType keyframe)
            {
                keyframes.erase(keyframe);
            }

            /// Removes all keyframes from this animation
            void ClearKeyframes()
            {
                keyframes.clear();
            }

        protected:
            set<KeyframeType> keyframes;

        };

        /// Forward declaration
        class AnimatorTimeline;

        /// Points at a BaseAnimation instance and contains time information.
        class AnimatorClip
        {
        public:
            friend class AnimatorTimeline;

            /// Sets the animation source for this clip.
            void SetAnimation(BaseAnimation* animation);

            /// Samples the animation associated with this clip at the current local time.
            template<class KeyframeType>
            KeyframeType Sample()
            {
                return (static_cast<Animation<KeyframeType>*>(anim))->GetSample(this);
            }

            /// Returns the global start time for this clip in milliseconds.
            Uint32 GetStartTime();

            /// Set the number of times this clip should repeat. Use values < 0 for infinite loops,
            /// or 0 for just playing once (no loops).
            void SetLoops(int n);

            /// Returns the number of times this clip should repeat.
            int GetLoops();

            /// Returns the duration of this clip in milliseconds. This includes additional time due to loops.
            /// Note that if this method returns a value < 0, the duration is infinite.
            int GetDuration();

            /// Returns the absolute duration of the source animation in milliseconds.
            Uint32 GetSourceDuration();

            /// Plays the clip on a given animator timeline, starting at startTime + timeline.clock.GetTime().
            void Play(AnimatorTimeline& timeline, int startTime = 0, bool removeOnFinish = true);

            /// Pauses the clip.
            void Pause();

            /// Unpauses the clip.
            void Resume();

            /// Returns true if the clip is paused, false otherwise.
            bool IsPaused();

            /// Stop playing this clip (removes it from the animator timeline).
            void Stop();

            /// Returns true if this clip has an associated animator timeline, otherwise returns false.
            bool IsPlaying();

            /// Stretches the local timeline by some scale factor. Note that a scale factor of 0 has the same effect as pausing the clip.
            void SetStretch(float sf);

            /// Returns the scale factor applied to the local timeline.
            float GetStretch();

            /// Get the current local time of this clip.
            Uint32 GetTime();

            /// Clears the number of times this clip has been looped and resets the local timeline to 0.
            void Reset();

            const bool operator<(const AnimatorClip& other)
            {
                return globalStartTime < other.globalStartTime;
            }

        private:
            /// Updates the local timeline. Returns true if the clip has finished playing.
            bool Update(Clock& globalClock);

            /// This clock maintains the local timeline.
            Clock localClock;

            /// The source animation.
            BaseAnimation* anim = nullptr;

            /// The time at which this clip starts on the global animator timeline, in milliseconds.
            Uint32 globalStartTime = 0;

            /// How many times this clip repeats. Use values < 0 for infinity, 0 for playing just once.
            int loops = 0;

            /// How many times this clip has looped.
            int timesLooped = 0;

            /// The animator timeline this clip is currently playing on. Null if the clip is not playing.
            AnimatorTimeline* playingTimeline = nullptr;

            /// Whether or not this clip should be automatically removed once it finishes.
            bool autoRemove = true;

        };

        /// Analogous to Renderer; you can add/register multiple clips to be played back at a specified time.
        class AnimatorTimeline
        {
        public:
            friend class AnimatorClip;

            ~AnimatorTimeline();

            /// Updates the global timeline and any clips that are playing.
            void Update(float deltaTime);

            /// Removes all clips from the timeline.
            void Clear();

            /// The clock that keeps track of time (the global timeline for all clips in this animator).
            Clock clock;

        private:
            /// Stops an animation that is currently playing, removing it from the timeline.
            void Stop(AnimatorClip* clip);

            /// Plays a clip and stops it once it's duration is reached. The startTime parameter is relative to the current time, in milliseconds.
            /// Set removeOnFinish to false if you do NOT wish to have the clip removed when it has finished playing.
            void Play(AnimatorClip* clip, int startTime = 0, bool removeOnFinish = true);

            /// Overloaded Play() method which transitions from one playing clip to another, then removes the previous clip.
            /// Transition time specifies how long the transition should be in seconds.
            void Play(AnimatorClip* previous, AnimatorClip* next, float transitionTime = 0.5f);

            /// Clips ordered by their global start time
            vector<AnimatorClip*> clips;

        };

    }

}

#endif // ANIMATOR_H
