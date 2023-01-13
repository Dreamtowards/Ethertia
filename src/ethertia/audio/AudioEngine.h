//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_AUDIOENGINE_H
#define ETHERTIA_AUDIOENGINE_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <ethertia/util/Log.h>



class AudioSource
{
public:
    ALuint m_SourceId = 0;

    AudioSource() {
        alGenSources(1, &m_SourceId);
    }
    ~AudioSource() {
        alDeleteSources(1, &m_SourceId);
    }

    void play() {
        alSourcePlay(m_SourceId);
    }
    void pause() {
        alSourcePause(m_SourceId);
    }
    void stop() {
        alSourceStop(m_SourceId);
    }

    void looping(bool loop) {
        alSourcei(m_SourceId, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }

    ALuint _state() {
        ALint val;
        alGetSourcei(m_SourceId, AL_SOURCE_STATE, &val);
        return val;
    }
    bool isPlaying() {
        return _state() == AL_PLAYING;
    }
    bool isPaused() {
        return _state() == AL_PAUSED;
    }
    bool isStopped() {
        return _state() == AL_STOPPED;
    }

    inline void _set(ALenum param, float f) {
        alSourcef(m_SourceId, param, f);
    }
    inline float _get_f(ALenum param) {
        float val;
        alGetSourcef(m_SourceId, param, &val);
        return val;
    }

    void setVolume(float f) {  _set  (AL_GAIN, f); }
    float getVolume() { return _get_f(AL_GAIN);    }

    void setPitch(float v) {  _set  (AL_PITCH, v); }
    float getPitch() { return _get_f(AL_PITCH);    }

    void setPosition(float x, float y, float z) {
        alSource3f(m_SourceId, AL_POSITION, x,y,z);
    }

    // velocity
    // direction

    // min, max gain.
    // cone inner,outer. cone gain.
    // max distance, reference distance.
    // rolloff factor




    void QueueBuffer(ALuint bufId) {
        alSourceQueueBuffers(m_SourceId, 1, &bufId);
    }
    void UnqueueBuffer() {
        alSourceUnqueueBuffers(m_SourceId, 1, nullptr); // todo: nullptr?
    }
    void UnqueueAllBuffers() {

    }

    int BuffersQueued() {
        ALint i;
        alGetSourcei(m_SourceId, AL_BUFFERS_QUEUED, &i);
        return i;
    }
    int BuffersProcessed() {
        ALint i;
        alGetSourcei(m_SourceId, AL_BUFFERS_PROCESSED, &i);
        return i;
    }


};

class AudioBuffer
{
public:
    ALuint m_BufferId = 0;
    int m_Frequence = 0;  // samples per sec.

    AudioBuffer() {
        alGenBuffers(1, &m_BufferId);
    }
    ~AudioBuffer() {
        alDeleteBuffers(1, &m_BufferId);
    }

    void buffer_data(ALuint format, void* data, size_t datalen, int freq) {
        m_Frequence = freq;
        alBufferData(m_BufferId, format, data, datalen, freq);
    }
};

class AudioEngine
{
public:

    ALCdevice*  m_Device = nullptr;
    ALCcontext* m_Context = nullptr;

    ALCdevice* m_CaptureDevice = nullptr;

    inline static const int CAPTURE_BUF_SIZE = 32768;
    inline static char CAPTURE_BUF[CAPTURE_BUF_SIZE];
    bool m_CaptureStarted = false;
    int m_CaptureSampleRate = 44100;  // 8000-96000

    AudioEngine()
    {
        BENCHMARK_TIMER;

        m_Device = alcOpenDevice(nullptr);
        if (!m_Device)
            throw new std::runtime_error("failed open al device.");

        m_Context = alcCreateContext(m_Device, nullptr);
        if (!m_Context)
            throw new std::runtime_error("failed create al context.");

        alcMakeContextCurrent(m_Context);

        {
            m_CaptureDevice = alcCaptureOpenDevice(nullptr, m_CaptureSampleRate, AL_FORMAT_MONO16, CAPTURE_BUF_SIZE);
            if (!m_CaptureDevice)
                throw std::runtime_error("Failed open capture device.");

        }

        Log::info("Init AL_{} | {} @{} .{}, Capt/ {}.\1",
                  alGetString(AL_VERSION),
                  alcGetString(m_Device, ALC_DEVICE_SPECIFIER),
                  alcGetString(m_Device, ALC_ALL_DEVICES_SPECIFIER),  // actual device name.
                  _infoOutputMode(m_Device),
                  alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));

        checkAlError("Init AL");
    }

    ~AudioEngine()
    {
        if (m_CaptureStarted)
            stopCapture();
        alcCloseDevice(m_CaptureDevice);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_Context);
        alcCloseDevice(m_Device);
    }

    void startCapture() {
        m_CaptureStarted = true;
        alcCaptureStart(m_CaptureDevice);
    }

    void stopCapture() {
        m_CaptureStarted = false;
        alcCaptureStop(m_CaptureDevice);
    }

    size_t sampleCapture(int16_t** buf, size_t capacity = 0) {
        ALCsizei samplesCaptured = 0;
        alcGetIntegerv(m_CaptureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesCaptured);
        if (samplesCaptured < 1) {
            return 0;
        }
        if (capacity != 0 && samplesCaptured > capacity) {
            samplesCaptured = capacity;
        }
        assert(samplesCaptured <= CAPTURE_BUF_SIZE);

        // MONO16 PCM.
        // OpenAL gives native-endian.
        alcCaptureSamples(m_CaptureDevice, CAPTURE_BUF, samplesCaptured);

        *buf = (int16_t*)CAPTURE_BUF;
        return samplesCaptured;
    }


    static const char* _AL_ErrorName(ALenum err) {
        switch (err) {
            case AL_INVALID_NAME: return "AL_INVALID_NAME";
            case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
            case AL_INVALID_VALUE:return "AL_INVALID_VALUE";
            case AL_INVALID_OPERATION:return "AL_INVALID_OPERATION";
            case AL_OUT_OF_MEMORY:return "AL_OUT_OF_MEMORY";
            default: return "unknown error";
        }
    }
    static void checkAlError(std::string_view phase) {
        ALenum err = alGetError();
        if (err) {
            Log::warn("OpenAL Error ## @{} ##: {} ({})", phase, err, _AL_ErrorName(err));
        }
    }

    // Master gain. value should be positive.
    void setVolume(float f) {
        alListenerf(AL_GAIN, f);
    }
    float getVolume() {
        float f;
        alGetListenerf(AL_GAIN, &f);
        return f;
    }



    // Listener

    void setPosition(float x, float y, float z) {
        alListener3f(AL_POSITION, x,y,z);
    }
    void setPosition(glm::vec3 p) {
        setPosition(p.x, p.y, p.z);
    }

    void setVelocity() {

    }

    void setOrientation(float x, float y, float z) {
        float orie[6] = {x, y, z, 0, 1, 0};  // FaceDir, UpDir(Y-Up)
        alListenerfv(AL_ORIENTATION, orie);
    }
    void setOrientation(glm::vec3 d) {
        setOrientation(d.x, d.y, d.z);
    }







    static const char* _infoOutputMode(ALCdevice* device) {
        const char* modename = "(error)";
        if(alcIsExtensionPresent(device, "ALC_SOFT_output_mode")) {
            ALCenum mode = 0;
            alcGetIntegerv(device, ALC_OUTPUT_MODE_SOFT, 1, &mode);

            switch(mode) {
                case ALC_ANY_SOFT: modename = "Unknown / unspecified"; break;
                case ALC_MONO_SOFT: modename = "Mono"; break;
                case ALC_STEREO_SOFT: modename = "Stereo (unspecified encoding)"; break;
                case ALC_STEREO_BASIC_SOFT: modename = "Stereo (basic)"; break;
                case ALC_STEREO_UHJ_SOFT: modename = "Stereo (UHJ)"; break;
                case ALC_STEREO_HRTF_SOFT: modename = "Stereo (HRTF)"; break;
                case ALC_QUAD_SOFT: modename = "Quadraphonic"; break;
                case ALC_SURROUND_5_1_SOFT: modename = "5.1 Surround"; break;
                case ALC_SURROUND_6_1_SOFT: modename = "6.1 Surround"; break;
                case ALC_SURROUND_7_1_SOFT: modename = "7.1 Surround"; break;
            }
        }
        return modename;
    }
};

#endif //ETHERTIA_AUDIOENGINE_H
