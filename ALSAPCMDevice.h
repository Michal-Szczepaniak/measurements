/* From https://github.com/TheSalarKhan/ALSA.CPP */

#ifndef ROOM_CORRECTION_ALSAPCMDEVICE_H
#define ROOM_CORRECTION_ALSAPCMDEVICE_H

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <string>

class ALSAPCMDevice {
public:
    ALSAPCMDevice(
        std::string deviceName,
        unsigned int sampleRate,
        unsigned int channels,
        unsigned int framesPerPeriod,
        snd_pcm_format_t format,
        enum _snd_pcm_stream type
    );

    bool open();
    void close();
    void* allocateBuffer();
    void* allocateBufferForSingleChannel();
    unsigned int getBytesPerFrame();
    unsigned int getFrames() const;
    unsigned int getChannels() const;
    unsigned int getState() const;
    int drain();
    int prepare();

protected:
    snd_pcm_t* _handle;
    std::string _deviceName;
    unsigned int _sampleRate, _channels;
    snd_pcm_uframes_t _framesPerPeriod;
    snd_pcm_format_t _format;
    enum _snd_pcm_stream _type;
};


#endif //ROOM_CORRECTION_ALSAPCMDEVICE_H
