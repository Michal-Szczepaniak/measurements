#include "SweepGenerator.h"
#include "ALSAPlaybackDevice.h"
#include "ALSACaptureDevice.h"
#include "Reformatter.h"
#include "CLI11.hpp"
#include <cstdint>
#include <thread>
#include <string>
#include <iostream>
#include "ImpulseResponseConverter.h"

void play(std::vector<int8_t> audio, ALSAPlaybackDevice* playbackDevice, int channel, bool *start) {
    int i = 0;
    unsigned int frames = playbackDevice->getFrames();
    auto buffer = (int8_t *) playbackDevice->allocateBufferForSingleChannel();
    unsigned int channels = playbackDevice->getChannels();
    unsigned int bytesPerFrame = playbackDevice->getBytesPerFrame()/channels;
    playbackDevice->prepare();

    while (audio.size() > i) {
        if (i != 0 && !*start) {
            *start = true;
        }

        for (int j = 0; j < frames * bytesPerFrame; j++, i++) {
            buffer[j] = audio[i];
        }

        playbackDevice->playFromBufferOnChannel(buffer, frames, bytesPerFrame, channel);
    }

    playbackDevice->drain();

    free(buffer);
}

void record(std::vector<int8_t>* audio, ALSACaptureDevice* captureDevice, unsigned int selectedCaptureChannel, bool* stop, bool *start) {
    unsigned int frames = captureDevice->getFrames();
    auto buffer = (int8_t *) captureDevice->allocateBuffer();
    unsigned int channelsCount = captureDevice->getChannels();
    unsigned int bytesPerFrame = captureDevice->getBytesPerFrame();
    *stop = false;
    while (!*start) continue;
    captureDevice->prepare();

    while (!*stop) {
        captureDevice->captureIntoBuffer(buffer, frames);
        for (int i = 0; i < frames * bytesPerFrame * channelsCount; i++) {
            if (channelsCount > 1 && i/bytesPerFrame % channelsCount != selectedCaptureChannel) {
                continue;
            }
            audio->push_back(buffer[i]);
        }
    }

    captureDevice->drain();

    free(buffer);
}

void save(const char *filename, std::vector<int8_t> file) {
    std::ofstream binaryFile (filename, std::ios::out | std::ios::binary);
    for (auto sample : file)
        binaryFile.put(sample);
    binaryFile.close();
}

std::vector<float> trim(std::vector<float> input) {
    std::vector<float> output;
    int i = 0;
    for (; i < input.size(); i++)
        if (input[i] != 0) break;

    for (; i < input.size(); i++)
        output.push_back(input[i]);

    return output;
}

static unsigned int calculatePeak(std::vector<float> input) {
    double peak = 0.f;
    unsigned int peakPosition = 0;

    for (int i = 0; i < input.size(); i++) {
        if (fabs(input[i]) > peak) {
            peak = fabs(input[i]);
            peakPosition = i;
        }
    }

    return peakPosition;
}

std::vector<float> alignImpulseResponseToPeak(std::vector<float> input, unsigned int peak) {
    std::vector<float> out;
    unsigned int inputPeak = calculatePeak(input);

    if (inputPeak > peak) {
        out = input;
        out.erase(out.begin(), out.begin()+(inputPeak - peak));
    } else if (inputPeak < peak) {
        for (int i = 0; i < peak - inputPeak; i++) {
            out.push_back(0);
        }
        out.insert(out.end(), input.begin(), input.end());
    } else {
        out = input;
    }

    return out;
}

std::vector<float> sumImpulseResponses(std::vector<float> input1, std::vector<float> input2) {
    std::vector<float> out;

    unsigned int peak1 = calculatePeak(input1);
    if (peak1 != calculatePeak(input2)) {
        input2 = alignImpulseResponseToPeak(input2, peak1);
    }

    for (int i = 0; i < std::max(input1.size(), input2.size()); i++) {
        float a = i >= input1.size() ? 0 : input1[i];
        float b = i >= input2.size() ? 0 : input2[i];
        out.push_back((a + b) / 2);
    }

    return out;
}

snd_pcm_format_t intFormatToAlsaFormat(unsigned int format) {
    switch (format) {
        case 16:
            return SND_PCM_FORMAT_S16;
        case 24:
            return SND_PCM_FORMAT_S24_3LE;
        case 32:
            return SND_PCM_FORMAT_S32;
        default:
            printf("Wrong format.");
            exit(0);
    }
}


typedef std::map<unsigned int, std::vector<std::vector<float>>> Recordings;

int main(int argc, char** argv) {
    unsigned int playbackChannels{};
    unsigned int captureChannels{};
    unsigned int selectedCaptureChannel{};
    unsigned int playbackFrames{};
    unsigned int captureFrames{};
    unsigned int rate{};
    unsigned int measurementsCount{};
    unsigned int sweepDuration{};
    unsigned int sweepSilence{};
    unsigned int freqStart{};
    unsigned int freqEnd{};
    unsigned int playbackFormat{};
    unsigned int captureFormat{};
    unsigned int subwooferChannel{};
    float subwooferVolume{};
    float sweepVolume{};
    std::string playbackDeviceName;
    std::string captureDeviceName;

    CLI::App app{"Measurement tool"};

    app.add_option("-P,--playback", playbackDeviceName, "Alsa device to play audio on")->required(true);
    app.add_option("-C,--capture", captureDeviceName, "Alsa device to capture audio on")->required(true);
    app.add_option("--playback-channels", playbackChannels, "Alsa playback device channel count")->default_val(2);
    app.add_option("--capture-channels", captureChannels, "Alsa capture device channel count")->default_val(1);
    app.add_option("--selected-capture-channel", selectedCaptureChannel, "Alsa capture device channel (we can record only one)")->default_val(0);
    app.add_option("--playback-frames", playbackFrames, "Alsa playback device frames")->default_val(64);
    app.add_option("--capture-frames", captureFrames, "Alsa capture device frames")->default_val(64);
    app.add_option("--playback-format", playbackFormat, "Alsa capture device frames")->default_val(16);
    app.add_option("--capture-format", captureFormat, "Alsa capture device frames")->default_val(24);
    app.add_option("-r,--rate", rate, "Rate, same for both")->default_val(48000);
    app.add_option("-m,--measurements", measurementsCount, "Number of measurement places")->default_val(1);
    app.add_option("-s,--freq-start", freqStart, "Start of sweep frequency")->default_val(20);
    app.add_option("-e,--freq-end", freqEnd, "End of sweep frequency")->default_val(20000);
    app.add_option("--sweep-duration", sweepDuration, "Duration of the sweep")->default_val(45);
    app.add_option("--sweep-silence", sweepSilence, "Silence before/after sweep")->default_val(2);
    app.add_option("-v,--sweep-volume", sweepVolume, "Volume of the sweep, quite arbitrary from 0-1")->default_val(0.1);
    app.add_option("--subwoofer-channel", subwooferChannel, "Subwoofer's channel/index")->default_val(5);
    app.add_option("--subwoofer-volume", subwooferVolume, "Volume of the subwoofer sweep, quite arbitrary from 0-1")->default_val(0.1);

    CLI11_PARSE(app, argc, argv);

    SweepGenerator sweepGenerator;
    auto generatedSweep = sweepGenerator.generate(rate, sweepVolume, freqStart, freqEnd, sweepDuration, sweepSilence, 0.05, 0.005);
    auto sweep = generatedSweep.first;
    auto inverseSweep = generatedSweep.second;
    auto generatedSubwooferSweep = sweepGenerator.generate(rate, subwooferVolume, freqStart, freqEnd, sweepDuration, sweepSilence, 0.05, 0.005);
    auto subwooferSweep = generatedSubwooferSweep.first;
    auto inverseSubwooferSweep = generatedSubwooferSweep.second;

    std::vector<int8_t> sweep16 = Reformatter::convertFromFloat(sweep, 16);
    std::vector<int8_t> subwooferSweep16 = Reformatter::convertFromFloat(subwooferSweep, 16);
    std::vector<int8_t> sweepRecordingBuffer{};

    ALSAPlaybackDevice playbackDevice(
        playbackDeviceName,
        rate,
        playbackChannels,
        playbackFrames,
        intFormatToAlsaFormat(playbackFormat)
    );
    ALSACaptureDevice captureDevice(
        captureDeviceName,
        rate,
        captureChannels,
        captureFrames,
        intFormatToAlsaFormat(captureFormat)
    );

    Recordings recordings;
    playbackDevice.open();
    captureDevice.open();
    playbackDevice.drain();
    captureDevice.drain();
    for (int measurements = 0; measurements < measurementsCount; measurements++) {
        for (int channel = 0; channel < playbackChannels; channel++) {
            bool stopRecording = false;
            bool startRecording = false;
            std::thread playbackThread(play, channel == subwooferChannel ? subwooferSweep16 : sweep16, &playbackDevice, channel, &startRecording);
            std::thread recordThread(record, &sweepRecordingBuffer, &captureDevice, selectedCaptureChannel, &stopRecording, &startRecording);
            std::this_thread::sleep_for(std::chrono::seconds(sweepDuration+(sweepSilence*2)));
            playbackThread.join();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            stopRecording = true;
            recordThread.join();

            recordings[channel].push_back(Reformatter::convertToFloat(sweepRecordingBuffer, 24));
            sweepRecordingBuffer.clear();
        }

        if (measurements + 1 != measurementsCount) {
            std::cout << "Press enter for next measurement" << std::endl;
            std::getchar();
        }
    }

    captureDevice.close();
    playbackDevice.close();

    for (auto it = recordings.begin(); it != recordings.end(); it++) {
        std::vector<float> ir = ImpulseResponseConverter::convert(
            recordings[it->first][0],
            it->first == subwooferChannel ? inverseSubwooferSweep : inverseSweep
        );

        for (int i = 1; i < it->second.size(); i++) {
            auto tmpIr = ImpulseResponseConverter::convert(
                recordings[it->first][i],
                it->first == subwooferChannel ? inverseSubwooferSweep : inverseSweep
            );

            ir = sumImpulseResponses(ir, tmpIr);
        }

        std::ostringstream oss;
        oss << "ir-" << it->first + 1 << ".raw";
        save(oss.str().c_str(), Reformatter::convertFromFloat(ir, 64));
        std::cout << "Saved: " << oss.str() << std::endl;
    }

    return 0;
}
