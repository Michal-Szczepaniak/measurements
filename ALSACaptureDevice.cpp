/* From https://github.com/TheSalarKhan/ALSA.CPP */

#include "ALSACaptureDevice.h"

ALSACaptureDevice::ALSACaptureDevice(
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
        SND_PCM_STREAM_CAPTURE)
{}

unsigned int ALSACaptureDevice::captureIntoBuffer(void* buffer, snd_pcm_uframes_t framesToCapture) {
    if(framesToCapture != _framesPerPeriod) {
        fprintf(stderr, "frames_to_read must equal frames in period <%lu>\n", _framesPerPeriod);
        return 0;
    }

    snd_pcm_sframes_t framesRead = snd_pcm_readi(_handle, buffer, framesToCapture);

    if(framesRead == 0) {
        fprintf(stderr, "End of file.\n");
        return 0;
    }

    if(framesRead != _framesPerPeriod) {
        fprintf(stderr, "Short read: we read <%ld> frames\n", framesRead);
        // A -ve return value means an error.
        if(framesRead < 0) {
            fprintf(stderr, "error from readi: %s\n", snd_strerror(framesRead));
            return 0;
        }
        return framesRead;
    }

    return 0;
}