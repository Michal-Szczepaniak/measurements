#ifndef ROOM_CORRECTION_REFORMATTER_H
#define ROOM_CORRECTION_REFORMATTER_H

#include <vector>
#include <limits>
#include <cmath>
#include <cstdint>

class Reformatter {
public:
    Reformatter() = delete;

    template<typename OutputType>
    static std::vector<OutputType> convertFromFloat(const std::vector<float>& input) {
        OutputType max = std::numeric_limits<OutputType>::max();
        std::vector<OutputType> output;

        for (float v : input)
            output.push_back(std::round(v*max));

        return output;
    }

    static std::vector<int8_t> convertFromFloat(const std::vector<float>& input, int8_t sampleSize) {
        std::vector<int8_t> output;

        switch (sampleSize) {
            case 8:
                output = convertFromFloat<int8_t>(input);
                break;
            case 16: {
                auto tmp = convertFromFloat<int16_t>(input);

                union {
                    int16_t input;
                    int8_t output[2];
                } conv{};

                for (int16_t v: tmp) {
                    conv.input = v;
                    output.push_back(conv.output[0]);
                    output.push_back(conv.output[1]);
                }
            }
                break;
            case 32: {
                auto tmp = convertFromFloat<int32_t>(input);

                union {
                    int32_t input;
                    int8_t output[4];
                } conv{};

                for (int32_t v: tmp) {
                    conv.input = v;
                    output.push_back(conv.output[0]);
                    output.push_back(conv.output[1]);
                    output.push_back(conv.output[2]);
                    output.push_back(conv.output[3]);
                }
            }
                break;
            case 64: { // float
                union {
                    float input;
                    int8_t output[4];
                } conv{};

                for (float v: input) {
                    conv.input = v;
                    output.push_back(conv.output[0]);
                    output.push_back(conv.output[1]);
                    output.push_back(conv.output[2]);
                    output.push_back(conv.output[3]);
                }
            }
                break;
        }

        return output;
    }

    static std::vector<float> convertToFloat(const std::vector<int8_t>& input, uint8_t sampleSize) {
        std::vector<float> output;
        unsigned int sampleBytes = sampleSize/8;

        for (int i = 0; i < input.size(); i+= sampleBytes) {
            union {
                int8_t input[4];
                int32_t output;
            } conv;

            for (int j = 0; j < 4; j++) {
                if (j >= 4 - sampleBytes) {
                    conv.input[j] = input[i+(j-(4-sampleBytes))];
                } else {
                    if (input[i] & 0b10000000) {
                        conv.input[j] = -1;
                    } else {
                        conv.input[j] = 0;
                    }
                }
            }

            output.push_back(((float)conv.output/(float)std::numeric_limits<int32_t>::max())*10.f);
        }

        return output;
    }

};


#endif //ROOM_CORRECTION_REFORMATTER_H
