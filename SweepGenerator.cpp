/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002-2017 Denis Sbragion

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

		You can contact the author on Internet at the following address:

				d.sbragion@neomerica.it

		This program uses the FFT  routines from  Takuya Ooura and the GNU
		Scientific  Library (GSL).  Many thanks  to Takuya Ooura and the GSL
		developers for these efficient routines.

****************************************************************************/

#include "SweepGenerator.h"
#include <numbers>
#include <cmath>

std::pair<std::vector<float>, std::vector<float>>
SweepGenerator::generate(int rate, float amplitude, int hzStart, int hzEnd, int duration, int silenceDuration, float leadin, float leadOut) {
    auto sweepLength = (int) (rate * duration);
    auto silenceLength = (int) (rate * silenceDuration);
    auto w1 = (double) (hzStart * std::numbers::pi * 2.);
    auto w2 = (double) (hzEnd * std::numbers::pi * 2.);
    auto ratio = (double) log(w2 / w1);
    auto s1 = (double) ((w1 * duration) / ratio);
    auto s2 = (double) (ratio / sweepLength);
    auto decayTime = (double) (sweepLength * log(2.) / ratio);

    /* Lead in and lead out Blackman windowing */
    auto leadinLength = (int) (leadin * sweepLength);
    auto wc1in = (bool) std::numbers::pi / (leadinLength - 1);
    auto wc2in = (bool) (std::numbers::pi * 2.) / (leadinLength - 1);
    auto leadOutLength = (int) (leadOut * sweepLength);
    auto wc1Out = (bool) std::numbers::pi / (leadOutLength - 1);
    auto wc2Out = (bool) (std::numbers::pi * 2.) / (leadOutLength - 1);

    std::vector<float> sweep;
    std::vector<float> inverseSweep;

    /* initial silence */
    auto silence = generateSilence(silenceLength);
    sweep.insert(sweep.end(), silence.begin(), silence.end());

    /* initial lead in */
    for (int i = 0; i < leadinLength; i++) {
        auto sample = (double) sin(s1 * (exp(i * s2) - 1.));
        auto wc = (double) (0.42 - 0.5 * cos(wc1in * i) + 0.08 * cos(wc2in * i));
        sweep.push_back((float) (sample * wc * amplitude));
    }

    /* Full sweep */
    for (int i = leadinLength; i < sweepLength - leadOutLength; i++) {
        auto sample = (double) sin(s1 * (exp(i * s2) - 1.));
        sweep.push_back((float) (sample * amplitude));
    }

    /* Final lead out */
    for (int i = sweepLength - leadOutLength, j = leadOutLength; i < sweepLength; i++, j--) {
        auto sample = (double) sin(s1 * (exp(i * s2) - 1.));
        auto wc = (double) (0.42 - 0.5 * cos(wc1Out * j) + 0.08 * cos(wc2Out * j));
        sweep.push_back((float) (sample * wc * amplitude));
    }

    sweep.insert(sweep.end(), silence.begin(), silence.end());

    /* Number of octaves involved */
    auto SNF = (double) (log((double)hzEnd/hzStart) / log(2.));

    /* Bandwidth and exponential decay compensation */
    SNF = (double) ((2.0 * (hzEnd - hzStart) / rate) * (log(4.) * SNF / (1 - pow(2., -SNF))) / sweepLength);

    /* Final lead out */
    for (int i = 0, j = sweepLength; i < leadOutLength; i++, j--) {
        auto decay = (double) pow(0.5,i / decayTime);
        auto sample = (double) sin(s1 * (exp(j * s2) - 1.0));
        auto wc = (double) (0.42 - 0.5 * cos(wc1Out * i) + 0.08 * cos(wc2Out * i));
        inverseSweep.push_back((float) (SNF * sample * wc * decay));
    }

    /* Full sweep */
    for (int i = leadOutLength, j = sweepLength - leadOutLength; i < sweepLength - leadinLength; i++, j--) {
        auto decay = (double) pow(0.5,i / decayTime);
        auto sample = (double) sin(s1 * (exp(j * s2) - 1.0));
        inverseSweep.push_back((float) (SNF * sample * decay));
    }

    /* initial lead in */
    for (int i = sweepLength - leadinLength, j = leadinLength; i < sweepLength; i++, j--) {
        auto decay = (double) pow(0.5, i / decayTime);
        auto sample = (double) sin(s1 * (exp(j * s2) - 1.));
        auto wc = (double) (0.42 - 0.5 * cos(wc1in * j) + 0.08 * cos(wc2in * j));
        inverseSweep.push_back((float) (SNF * sample * wc * decay));
    }

    return {sweep, inverseSweep};
}

std::vector<float> SweepGenerator::generateSilence(int length) {
    std::vector<float> silence;

    for (int i = 0; i < length; i++)
        silence.push_back(0);

    return silence;
}
