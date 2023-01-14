//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_AUDIOBUFFER_H
#define ETHERTIA_AUDIOBUFFER_H


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

#endif //ETHERTIA_AUDIOBUFFER_H
