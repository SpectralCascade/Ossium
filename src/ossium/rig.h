#ifndef RIG_H
#define RIG_H

#include <string>
#include <vector>
#include <map>

#include "texture.h"
#include "transform.h"

using namespace std;

namespace ossium
{

    /// Part of a skeleton, like a real joint
    struct Joint
    {
        /// Name of the joint
        string name;
        /// Inverse of the bind (reference) pose transform for this joint
        Transform inverseBindPose;
        /// Index of parent joint in array
        Uint8 parent;
    };

    /// A specific pose for a skeleton
    struct Pose
    {
        /// Pose of joints in local space (relative to parent)
        Transform* localPoses;
        /// Pose of joints in global space (relative to the bind pose)
        Transform* globalPoses;
    };

    /// A single keyframe of animation for a skeleton
    struct PoseAnimationKeyframe
    {
        /// Skeleton pose for this keyframe
        Pose* pose;
        /// Keyframe time in the animation sequence
        Uint32 time;
    };

    /// Contains an array of keyframes containing pose and sprite state data
    class PoseAnimationClip
    {
    public:
        PoseAnimationClip();
        ~PoseAnimationClip();

        /// Frees all keyframes
        void free();

        /// Adds a single pose animation keyframe
        void addKeyframe(Pose pose, Uint32 time);

        /// Returns an interpolation between the current animation pose and the next
        /// or just the current pose (floored interpolation value)
        Pose* getPose(Uint32 time = 0, bool interpolate);

        /// Name of the clip, e.g. "idle_1", "walk", "run" etc.
        string name;

    private:
        /// Array of keyframes, sorted in the order they should play in according to time
        vector<PoseAnimationKeyframe> keyframes;

        /// Current pose of animation
        unsigned int currentPose;

    };

    /// A skeleton class made up of joints, a reference pose and pose animation clip data
    /// This can be used with a mesh, skin or something else that uses skeletal animation.
    /// Skeleton is a type of resource, so instances can be controlled with a resource controller object
    class Skeleton
    {
    public:
        Skeleton();
        ~Skeleton();

        /// Load resource method
        bool load(string guid_path);

        /// Post-load initialisation
        bool init();

        /// Frees all data accumulated by this skeleton
        void free();

        /// Frees all the animation clips, but not the joints
        void freeClips();

        /// Adds a single animation clip to the skeleton
        void addClip(PoseAnimationClip clip);
        /// Overload turns data into a clip
        void addClip(string name, const PoseAnimationKeyframe* keyframes);

        /// Plays an animation clip forever, until it is stopped or called with another clip name
        /// Transition time is the time allowed for a transition to take place in seconds
        /// 0 transition time means an instant transition, any time higher than this utilises interpolation
        void play(string clipName, float transitionTime = 0.1f);
        /// Loops an animation clip; if given a value of 0, loops forever, if given 1, just plays once then stops
        void loop(string clipName, float transitionTime = 0.1f);

        /// Updates the skeleton's current pose of animation
        void update(Uint32 time);

        /// Returns number of joints in the skeleton
        int getJointCount();

        /// Get/set interpolation
        bool isInterpolating();
        void setInterpolation(bool lerp = true);

        /// Draws all the joints in the skeleton as circles in a given pose (NULL default to T-pose)
        void drawJoints(Renderer* renderer, int x, int y, SDL_Color color);
        /// Draws all the bones between joints as lines in a given pose (NULL default to T-Pose)
        void drawBones(Renderer* renderer, int x, int y, SDL_Color color);

    private:
        /// Number of joints in skeleton
        int jointCount;

        /// Array of joints
        Joint* joints;

        /// Animation clips consisting of keyframe poses for the skeleton
        vector<PoseAnimationClip> clips;

        /// Current animation clip
        PoseAnimationClip* currentClip;

        /// Default animation clip index in animation clip array
        unsigned int defaultClip;

        /// Pointer to current animation keyframe; if NULL, assume bind pose
        PoseAnimationKeyframe* currentKeyframe;

        /// When true, interpolates between keyframe poses. Otherwise, expect some bumpy animation!
        bool interpolate;

    };

    /// A skin class made up of sprites that are attached to a skeleton;
    /// not a vertex mesh, but it's something
    /// Component resource - requires a skeleton to attach sprites to joints
    class SpriteSkin
    {
    public:
        SpriteSkin();
        ~SpriteSkin();

    private:
        /// Sprites to use
        StateSprite* sprites;

        /// The skeleton this skin is attached to
        Skeleton* skelly;

    };

    /// The Ossium Rig class, for 2D skeletal animation; holds everything together!
    /// Used for anything that can make better use of skeletal animation than purely sprite-based animation
    class Rig
    {
    public:
        Rig();
        ~Rig();

    private:
        /// The skeleton to use for this rig
        Skeleton* skelly;

    };

}

#endif // RIG_H
