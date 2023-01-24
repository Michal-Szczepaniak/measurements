/* From https://github.com/TheSalarKhan/ALSA.CPP */

#include "ALSAPlaybackDevice.h"

ALSAPlaybackDevice::ALSAPlaybackDevice(
        std::string deviceName,
        unsigned int sampleRate,
        unsigned int channels,
        unsigned int framesPerPeriod,
        snd_pcm_format_t format
) : ALSAPCMDevice(
        deviceName,
        sampleRate,
        channels,
        framesPerPeriod,
        format,
        SND_PCM_STREAM_PLAYBACK)
{}

long ALSAPlaybackDevice::playFromBuffer(void* buffer, snd_pcm_uframes_t framesToPlay) {
    if(framesToPlay != _framesPerPeriod) {
        fprintf(stderr, "frames_to_play must equal frames in period <%lu>\n", _framesPerPeriod);
        return 0;
    }

    bool done = false;

    snd_pcm_sframes_t framesWritten = snd_pcm_writei(_handle, buffer, framesToPlay);

    if (framesWritten == -EPIPE) {
        /* EPIPE means underrun */
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(_handle);
    } else if (framesWritten < 0) {
        fprintf(stderr, "error from writei: %s\n", snd_strerror(framesWritten));
    }  else if (framesWritten != _framesPerPeriod) {
        fprintf(stderr, "short write, write %ld frames\n", framesWritten);
    }

    return framesWritten;
}
