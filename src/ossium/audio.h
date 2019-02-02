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

    /// Mix-in class for doing stuff with volume. Requires Derived to implement OnVolumeChanged()
    ///
    /// !!! TODO !!!
    ///
    /// Actually implement correct dB -> percentage and vice-versa conversions. Atm it's all just a direct mapping of ranges
    ///
    template<class Derived>
    class VolumeControl
    {
    public:
        /// Initialise values
        VolumeControl()
        {
            volume = 1.0f;
            targetVolume = 1.0;
        }

        /*/// Sets the volume level of the audio stream in decibels, where 0 = max volume, -80.0f = silent
        /// Values are clamped between -80dB and 0dB
        void SetLevel(float dB = 0.0f);
        {
            derived()->OnVolumeChanged();
        }
        /// Returns the volume level of the audio stream in decibels, where 0 = max volume, -80.0f = silent
        float GetLevel();*/

        /// Sets the linear volume level of the audio stream as a normalised value between 0 and 1.
        /// Any values outside of the 0 - 1 range are clamped
        void SetVolume(float percent = 1.0f)
        {
            volume = clamp(percent, 0.0f, 1.0f);
            derived()->OnVolumeChanged();
        }
        /// This method is very similar to SetLevel(), which is on a logarithmic decibel scale but this method is normalised between 0 and 1.
        /// Any values outside of the 0 - 1 range are clamped
        void SetVolumeLogarithmic(float percent = 1.0f)
        {
            volume = clamp(percent, 0.0f, 1.0f);
            derived()->OnVolumeChanged();
        }
        /// Returns the linear volume level of the audio stream as a normalised value between 0 and 1.
        float GetVolume()
        {
            return volume;
        }
        /// This method is very similar to GetLevel(), which is on a logarithmic decibel scale but this method is normalised between 0 and 1.
        float GetVolumeLogarithmic()
        {
            return volume;
        }

/*        /// Fades to a specified target volume given a change in time. Fade time is 1 second by default
        void Fade(float targetVolumePercent, float seconds = 1.0f);

        /// Fade in to maximum volume
        void FadeIn(float seconds);
        /// Fade out to silence
        void FadeOut(float seconds);

        /// Is the audio stream fading?
        bool IsFading();*/

        /// Returns the target volume. If the audio isn't fading this should be the same as volume
        float GetTargetVolume()
        {
            return targetVolume;
        }

        /// CRTP convenience method
        Derived* derived()
        {
            return reinterpret_cast<Derived*>(this);
        }

    private:
        /// Normalised linear volume level
        float volume;

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

    }

    /// This represents an audio bus like those you'd get on a mixing desk, with multiple inputs and a single output
    class AudioBus : public VolumeControl<AudioBus>
    {
    public:
        friend class VolumeControl<AudioBus>;
        friend class AudioSource;

        /// Initialise stuff
        AudioBus();

        /// Sets the name of this audio channel
        void SetName(string setName);
        /// Returns the name of this audio channel
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

    private:
        /// When the volume changes, iterate over all the inputs and call their OnVolumeChanged() methods
        void OnVolumeChanged();

        /// The next bus in the signal chain that this bus is linked to; if null, this bus is the end of a signal chain
        AudioBus* linkedBus;

        /// The set of input buses coming into this audio bus
        set<AudioBus*> input_buses;
        /// The set of input signals coming into this bus
        set<AudioSource*> input_signals;

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
    class AudioSource : public VolumeControl<AudioSource>
    {
    public:
        friend class AudioBus;
        friend class AudioInternals::ChannelController;

        AudioSource();
        ~AudioSource();

        /// Links this source to an audio output bus
        void Link(AudioBus* bus);

        /// Unlinks this audio source from the linked audio bus, if any
        void Unlink();

        /// Plays the specified audio clip; repeats specifies the number of times to repeat, where values < 0 = infinity
        void Play(AudioClip* sample, float vol = 1.0f, int repeats = 0);

        /// Whether or not this audio source is currently playing anything
        bool IsPlaying();

        /// Whether or not this audio source is linked to an output bus
        bool IsLinked();

        /// Returns the true output volume of this audio source, with all linked bus volume levels applied
        float GetFinalVolume();

        /// When any volume value changes in the signal chain, this sets the true output volume of this source
        void OnVolumeChanged();

    protected:
        void OnPlayFinished();

    private:
        /// The linked audio output bus; if not linked, this is set to null
        AudioBus* linkedBus;

        /// The SDL_Mixer channel currently reserved for this audio source. If < 0, the audio source is not playing anything
        int channel_id;

        /// The total number of available channels
        static int total_channels;

        /// Convenient reference
        static AudioInternals::ChannelController& _channelController;

    };
/*
    namespace AudioInternals
    {

        /// Wrapper class for Mix_Music, which streams a single audio clip from disk rather than loading the whole clip into memory
        /// There can only be a single audio stream as there can only be one Mix_Music instance
        /// Audio played via AudioStream can potentially be higher quality than AudioClip
        class AudioStream : public VolumeControl<AudioStream>, Singleton<AudioStream>
        {
        public:
            AudioStream();
            ~AudioStream();

            /// Frees the audio stream
            void Free();

            /// Streams audio from a specified file path. Returns false if an error occurs. Otherwise caches the file path and starts playing
            /// Loops specifies the number of times to play the stream where -1 = infinity, 0 = never, 1 = once, 2 = twice and so on
            /// ms specifies the fade time in milliseconds. When ms = 0 there is no fade in
            bool Play(string path, int loops = 1, int ms = 0);
            /// Resumes the stream that is paused, or starts streaming from the cached audio file path. Returns false on failure
            /// Loops specifies the number of times to play the stream where -1 = infinity, 0 = never, 1 = once, 2 = twice and so on
            /// ms specifies the fade time in milliseconds. When ms = 0 there is no fade in
            bool Play(int loops = 1, int ms = 0);

            /// Pauses the audio stream if it is playing
            void Pause();
            /// Resumes the audio stream if it is paused
            void Resume();

            /// Stops streaming audio
            void Stop();

            /// Mutes the audio stream; this does not change the cached volume level
            void Mute();
            /// Unmutes the audio stream; this does not change the cached volume level
            void Unmute();

            /// It's not advised to use this, but if you need more SDL_Mixer features you can directly access the stream
            Mix_Music* _GetStream();

            /// Returns the cached path of the last or current file of the stream. Returns empty string if no file has been loaded successfully
            string GetPath();

            /// Updates any volume fading
            void Update();

        private:
            /// The Mix_Music instance
            Mix_Music* stream;

            /// The file path is cached
            string cachedPath;

            /// Whether the audio stream is paused
            bool paused;

            /// Whether the audio stream is started or stopped
            bool started;

            /// Whether or not the audio stream is muted
            bool muted;

            /// AudioStream has it's own delta instance so that the stream can be updated as many times as you want
            Delta delta;

        };

    }

    /// Convenient global reference for accessing the AudioStream singleton
    AudioInternals::AudioStream& SoundStream = AudioInternals::AudioStream::_Instance();

    /// Used to control volume levels and other effects on all audio channels, including the single AudioStream instance
    /// for which there is a dedicated static channel. This is the core of the audio system.
    class AudioMixer
    {
    public:
        AudioMixer();
        ~AudioMixer();

    private:

    };
*/
}

#endif // AUDIO_H
