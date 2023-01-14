//
// Created by Dreamtowards on 2023/1/5.
//

#ifndef ETHERTIA_AUDIOSOURCE_H
#define ETHERTIA_AUDIOSOURCE_H


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

#endif //ETHERTIA_AUDIOSOURCE_H
