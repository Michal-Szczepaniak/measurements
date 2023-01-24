/* From https://github.com/TheSalarKhan/ALSA.CPP */

#ifndef ROOM_CORRECTION_ALSAPLAYBACKDEVICE_H
#define ROOM_CORRECTION_ALSAPLAYBACKDEVICE_H

#include "ALSAPCMDevice.h"

class ALSAPlaybackDevice : public ALSAPCMDevice {
public:
    ALSAPlaybackDevice(
            std::string deviceName,
            unsigned int sampleRate,
            unsigned int channels,
            unsigned int framesPerPeriod,
            snd_pcm_format_t format
    );

    long playFromBuffer(void* buffer, snd_pcm_uframes_t framesToPlay);


    long
    playFromBufferOnChannel(const int8_t *buffer, snd_pcm_uframes_t framesToPlay, unsigned char frameSize, unsigned char channel) {
        if (channel >= _channels) {
            fprintf(stderr, "incorrect channel to play: %hhu\n", channel);
            return 0;
        }

        auto actualBuffer = (int8_t *) allocateBuffer();

        for (int i = 0, j = 0; i < framesToPlay * frameSize * _channels; i++) {
            if (i/frameSize % _channels == channel) {
                actualBuffer[i] = buffer[j];
                j++;
            } else {
                actualBuffer[i] = 0;
            }
        }

        long framesWritten = playFromBuffer(actualBuffer, framesToPlay);

        free(actualBuffer);

        return framesWritten;
    }
};


#endif //ROOM_CORRECTION_ALSAPLAYBACKDEVICE_H
