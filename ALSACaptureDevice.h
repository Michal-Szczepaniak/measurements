/* From https://github.com/TheSalarKhan/ALSA.CPP */

#ifndef ROOM_CORRECTION_ALSACAPTUREDEVICE_H
#define ROOM_CORRECTION_ALSACAPTUREDEVICE_H

#include "ALSAPCMDevice.h"

class ALSACaptureDevice : public ALSAPCMDevice {
public:
    ALSACaptureDevice(
        std::string deviceName,
        unsigned int sampleRate,
        unsigned int channels,
        unsigned int framesPerPeriod,
        snd_pcm_format_t format
    );

    unsigned int captureIntoBuffer(void* buffer, snd_pcm_uframes_t framesToCapture);
};


#endif //ROOM_CORRECTION_ALSACAPTUREDEVICE_H
