#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>
#include <SDL_mixer.h>

#include <set>
#include <vector>

#include "basics.h"
#include "delta.h"

using namespace std;

namespace ossium
{

    /// Mix-in class for doing stuff with volume and stereo panning. Requires Derived to implement OnVolumeChanged()
    template<class Derived>
    class AudioChannel
    {
    public:
        /// Initialise values
        AudioChannel()
        {
            volume = 1.0f;
            targetVolume = 1.0;
            panningAngle = 0;
        }

        /// Sets the volume level of the audio channel as a normalised value between 0 and 1.
        /// Any values outside of the 0 - 1 range are clamped
        void SetVolume(float vol)
        {
            volume = clamp(vol, 0.0f, 1.0f);
            derived()->OnVolumeChanged();
        }
        /// Returns the volume level of the audio channel as a normalised value between 0 and 1.
        float GetVolume()
        {
            return volume;
        }

        /// Sets stereo panning by direction. Note: this wraps around, so -90 is the same as 270 (left), etc.
        void SetPanning(Sint16 angle)
        {
            panningAngle = wrap(0, angle, 0, 360);
            derived()->OnVolumeChanged();
        }
        /// Returns the panning angle
        Sint16 GetPanning()
        {
            return panningAngle;
        }

        void SetStereoVolume(float vol, Sint16 angle)
        {
            volume = clamp(vol, 0.0f, 1.0f);
            panningAngle = wrap(0, angle, 0, 360);
            derived()->OnVolumeChanged();
        }

        /// Fades to a specified target volume given a change in time. Fade time is 1 second by default
        void Fade(float targetVolumePercent, float deltaTime, float seconds = 1.0f)
        {
            targetVolume = targetVolumePercent;
            bool increase = volume < targetVolume;
            if (volume != targetVolume)
            {
                float change = deltaTime * (1.0f / seconds);
                if (increase)
                {
                    volume += change;
                    if (volume > targetVolume)
                    {
                        volume = targetVolume;
                    }
                    derived()->OnVolumeChanged();
                }
                else
                {
                    volume -= change;
                    if (volume < targetVolume)
                    {
                        volume = targetVolume;
                    }
                    derived()->OnVolumeChanged();
                }
            }
        }

        /// Fade in to maximum volume
        void FadeIn(float deltaTime, float seconds = 1.0f)
        {
            Fade(1.0f, deltaTime, seconds);
        }
        /// Fade out to silence
        void FadeOut(float deltaTime, float seconds = 1.0f)
        {
            Fade(0.0f, deltaTime, seconds);
        }

        /// Is the audio stream fading?
        bool IsFading();

        /// Returns the target volume. If the audio isn't fading this should be the same as volume
        float GetTargetVolume()
        {
            return targetVolume;
        }

        /// CRTP convenience method
        Derived* derived()
        {
            return static_cast<Derived*>(this);
        }

    private:
        /// Normalised linear volume level
        float volume;

        /// The angle at which the audio is panned. 0 = centre, 90 = right, 180 = behind, 270 = left
        Sint16 panningAngle;

        /// The current target volume level
        float targetVolume;

    };


    /// Forward declaration
    class AudioSource;

    namespace AudioInternals
    {

        void OnAnyChannelFinished(int id);

        /// Controls the channels allocated by SDL_Mixer
        class ChannelController : public Singleton<ChannelController>
        {
        public:
            /// Allocates the specified number of audio channels for use with SDL_Mixer
            ChannelController();
            /// Cleans up all channels
            ~ChannelController();

            /// Allocates the specified number of SDL_Mixer channels
            void Init(int num_channels = 50);

            /// Deallocates all channels
            void Clear();

            /// Returns the first available channel and prevents it being reserved again. Returns -1 if it fails to find a free channel or the callback argument is null
            int ReserveChannel(AudioSource* src);
            /// Frees the specified channel; pass in a value < 0 to free all channels. Does not halt the freed channel(s), use HaltChannel() for that
            void FreeChannel(int id);

            /// Calls the callback associated with the id
            void ChannelFinished(int id);

        private:
            /// All reserved channels have a callback object which is called when the channel finishes playback; if unreserved, callback is null
            vector<AudioSource*> channels;

            /// Total channels allocated
            int numChannels = 0;

            /// Used for stepping through the array of channels
            int counter = 0;

        };

        /// Forward declaration
        class AudioStream;

    }

    /// This represents an audio bus like those you'd get on a mixing desk, with multiple inputs and a single output
    class AudioBus : public AudioChannel<AudioBus>
    {
    public:
        friend class AudioChannel<AudioBus>;
        friend class AudioSource;
        friend class AudioInternals::AudioStream;

        /// Initialise stuff
        AudioBus();

        /// Sets the name of this audio channel
        void SetName(string setName);
        /// Returns the name of this audio bus
        string GetName();

        /// Links this bus to an audio bus. Cannot link to more than 1 input bus;
        /// Unlinks current bus if already linked to an input bus and connects to the new one instead
        void Link(AudioBus* bus);

        /// Unlinks this output bus from the linked audio bus, if any
        void Unlink();

        /// Returns the audio bus this signal is linked to, or null if not linked
        AudioBus* GetLinkedBus();

        /// Convenience method for checking if this audio signal is linked to an audio bus or not
        bool IsLinked();

        /// Returns the final output volume of the root audio signal by recursion
        float GetFinalVolume();

        /// Returns the final stereo panning angle of the root audio signal by recursion
        Sint16 GetFinalPanning();

        /// Turn bypass mode on or off
        void SetBypassMode(bool bypass_channel);

        /// Is bypass mode on?
        bool IsBypassed();

        /// Is this bus muted?
        bool IsMuted();

        /// If the audio bus is muted, all inputs will also be muted
        void Mute();
        void Unmute();

    private:
        /// When the volume changes, iterate over all the input signals and call their OnVolumeChanged() methods
        void OnVolumeChanged();

        /// Whether or not volume and panning set by this bus should be ignored
        bool bypass;

        /// Whether or not this bus is muted
        bool muted;

        /// The next bus in the signal chain that this bus is linked to; if null, this bus is the end of a signal chain
        AudioBus* linkedBus;

        /// The set of input buses coming into this audio bus
        set<AudioBus*> input_buses;
        /// The set of input signals coming into this bus
        set<AudioSource*> input_signals;

        /// The audio stream coming into this bus; usually null unless the AudioStream has been linked
        AudioInternals::AudioStream* input_stream;

        /// Name of this audio bus
        string name;

    };

    /// Wrapper class for Mix_Chunk, representing an audio sample resource
    class AudioClip
    {
    public:
        AudioClip();
        ~AudioClip();

        void free();

        bool load(string guid_path, int* loadArgs = nullptr);
        bool init();

        /// Returns the audio chunk
        Mix_Chunk* GetChunk();

    private:
        NOCOPY(AudioClip);

        /// The actual audio file in memory
        Mix_Chunk* audioChunk;

    };

    /// Represents a generic audio source
    /// Not an audio bus by itself as it has no inputs, but it is a type of audio signal
    /// which may be fed into audio buses
    class AudioSource : public AudioChannel<AudioSource>
    {
    public:
        friend class AudioBus;
        friend class AudioInternals::ChannelController;

        AudioSource();
        virtual ~AudioSource();

        /// Links this source to an audio output bus
        void Link(AudioBus* bus);

        /// Unlinks this audio source from the linked audio bus, if any
        void Unlink();

        /// Plays the specified audio clip; vol specifies volume between 0 and 1, where values < 0 keeps the current volume of the source
        /// repeats specifies the number of times to repeat, where values < 0 = infinity
        void Play(AudioClip* sample, Sint16 panning, float vol = -1.0f, int repeats = 0);

        /// Simplified overload
        void Play(AudioClip* sample, float vol = -1.0f, int repeats = 0);

        /// Pauses this audio source if it is currently playing
        void Pause();

        /// Returns true if the audio source has been paused
        bool IsPaused();

        /// Resumes playing the audio source if anything is currently paused
        void Resume();

        /// Whether or not this audio source is currently playing anything
        bool IsPlaying();

        /// Whether or not this audio source is linked to an output bus
        bool IsLinked();

        /// Returns the true output volume of this audio source, with all linked bus volume levels applied
        float GetFinalVolume();

        /// Returns the true stereo panning of this audio source, with all linked bus panning angles applied
        Sint16 GetFinalPanning();

        /// When any volume value changes in the signal chain, this sets the true output volume of this source
        virtual void OnVolumeChanged();

    protected:
        void OnPlayFinished();

        /// Whether or not this audio source is paused
        bool paused;

        /// The linked audio output bus; if not linked, this is set to null
        AudioBus* linkedBus;

    private:
        /// The SDL_Mixer channel currently reserved for this audio source. If < 0, the audio source is not playing anything
        int channel_id;

        /// The total number of available channels
        static int total_channels;

        /// Convenient reference
        static AudioInternals::ChannelController& _channelController;

    };

    namespace AudioInternals
    {

        /// Callback when the music is finished
        void MusicFinished();

        /// Wrapper class for Mix_Music, which streams a single audio clip from disk rather than loading the whole clip into memory
        /// There can only be a single audio stream as there can only be one Mix_Music instance
        /// Audio played via AudioStream can potentially be higher quality than AudioClip, hence it's probably best used for music and the like
        /// Inherits from AudioBus so that it may be used in the mixer, rather than AudioSource which is designed to work with SDL_Mixer channels
        class AudioStream : public AudioChannel<AudioStream>, public Singleton<AudioStream>
        {
        public:
            AudioStream();
            virtual ~AudioStream();

            /// Resets started and paused to false
            void Init();

            /// Frees the audio stream
            void Free();

            /// Loads an audio file from disk; returns false on error
            bool Load(string path);

            /// Plays loaded audio as many times as specified by the loops argument, or forever if loops = -1
            void Play(float vol = -1.0f, int loops = 1);

            /// Loads an audio file and then immediately starts playing
            void Play(string path, float vol = -1.0f, int loops = 1);

            /// Pauses the stream if it is currently playing
            void Pause();

            /// Resumes playing the audio stream if anything is currently paused
            void Resume();

            /// Whether or not this audio stream is started
            bool IsStarted();

            /// Whether or not this audio stream is currently paused
            bool IsPaused();

            /// Stops streaming audio
            void Stop();

            /// Links this stream to an audio output bus
            void Link(AudioBus* bus);

            /// Unlinks this stream from the linked audio bus, if any
            void Unlink();

            /// Whether or not this audio stream is linked to an audio bus
            bool IsLinked();

            /// It's not advised to use this, but if you need more SDL_Mixer features you can directly access the stream
            Mix_Music* GetStream();

            /// Returns the cached path of the last or current file of the stream. Returns empty string if no file has been loaded successfully
            string GetPath();

            /// Always returns 0 as the audio stream is not designed for panning
            const Sint16 GetFinalPanning();

            /// Returns the true volume of the audio stream when all the linked bus volume levels have been applied
            float GetFinalVolume();

            /// When the volume changes, applies the true volume level
            void OnVolumeChanged();

        private:
            /// The Mix_Music instance
            Mix_Music* stream;

            /// The audio bus this stream is linked to, if at all
            AudioBus* linkedBus;

            /// The file path is cached
            string cachedPath;

            /// Whether the audio stream is started or not
            bool started;

            /// Whether the audio stream is paused or not
            bool paused;

        };

    }

    /// Convenient global reference for accessing the AudioStream singleton
    namespace global
    {

        extern AudioInternals::AudioStream& SoundStream;

    }

    /// Used to control volume levels and other effects on all audio channels, including the single AudioStream instance
    /// for which there is a dedicated static channel. This is the core of the audio system.
    /*class AudioMixer
    {
    public:
        AudioMixer();
        ~AudioMixer();

    private:

    };*/

}

#endif // AUDIO_H