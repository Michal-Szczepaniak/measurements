/* From https://github.com/TheSalarKhan/ALSA.CPP */

#include "ALSAPCMDevice.h"

ALSAPCMDevice::ALSAPCMDevice(
        std::string deviceName,
        unsigned int sampleRate,
        unsigned int channels,
        unsigned int framesPerPeriod,
        snd_pcm_format_t format,
        enum _snd_pcm_stream type
) :
        _deviceName(deviceName),
        _sampleRate(sampleRate),
        _channels(channels),
        _framesPerPeriod(framesPerPeriod),
        _format(format),
        _type(type)
{}

bool ALSAPCMDevice::open() {
    snd_pcm_hw_params_t *params;
    /* Open rc device. */
    int rc = snd_pcm_open(&_handle, _deviceName.c_str(), _type, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open rc device: %s\n", snd_strerror(rc));
        return false;
    }

    snd_pcm_hw_params_alloca(&params);

    snd_pcm_hw_params_any(_handle, params);

    if ((rc = snd_pcm_hw_params_set_access(_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
        printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(rc));

    if ((rc = snd_pcm_hw_params_set_format(_handle, params, _format)) < 0)
        printf("ERROR: Can't set format. %s\n", snd_strerror(rc));

    if ((rc = snd_pcm_hw_params_set_channels(_handle, params, _channels)) < 0)
        printf("ERROR: Can't set channels number. %s\n", snd_strerror(rc));

    if ((rc = snd_pcm_hw_params_set_rate_near(_handle, params, &_sampleRate, NULL)) < 0)
        printf("ERROR: Can't set rate. %s\n", snd_strerror(rc));

    if ((rc = snd_pcm_hw_params_set_period_size_near(_handle, params, &_framesPerPeriod, NULL)) < 0)
        printf("ERROR: Can't set period size. %s\n", snd_strerror(rc));

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(_handle, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        return false;
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &_framesPerPeriod, NULL);

    return true;
}

void ALSAPCMDevice::close() {
    snd_pcm_drain(_handle);
    snd_pcm_close(_handle);
}

void* ALSAPCMDevice::allocateBuffer() {
    unsigned int sizeOfOneFrame = getBytesPerFrame();
    return calloc(_framesPerPeriod, sizeOfOneFrame);
}

void* ALSAPCMDevice::allocateBufferForSingleChannel() {
    unsigned int sizeOfOneFrame = getBytesPerFrame();
    return calloc(_framesPerPeriod, sizeOfOneFrame/_channels);
}

unsigned int ALSAPCMDevice::getBytesPerFrame() {
    unsigned int sizeOfOneFrame = (snd_pcm_format_width(_format)/8) * _channels;
    return sizeOfOneFrame;
}

unsigned int ALSAPCMDevice::getFrames() const {
    return _framesPerPeriod;
}

unsigned int ALSAPCMDevice::getChannels() const {
    return _channels;
}

unsigned int ALSAPCMDevice::getState() const {
    return snd_pcm_state(_handle);
}

int ALSAPCMDevice::drain() {
    int rc = snd_pcm_drain(_handle);
    if (rc < 0) {
        fprintf(stderr, "unable to drain: %s\n", snd_strerror(rc));
    }

    return rc;
}

int ALSAPCMDevice::prepare() {
    int rc = snd_pcm_prepare(_handle);
    if (rc < 0) {
        fprintf(stderr, "unable to prepare: %s\n", snd_strerror(rc));
    }

    return rc;
}
