# Description

Measurements (random/temporary name) is a software to aid in generation of measurements and averaging of them. It produces Impulse responses for each speaker which then can be used in [drc-fir](https://drc-fir.sourceforge.net/) to create FIR Room Correction filters.

# Building

Cmake and make

# Usage

This software uses Alsa. Provide playback and recording devices along with correct formats. Additionally provide channels, volume and number of measurements. Rest should work just fine.

# License
This software is licensed under GPLv3. It uses [CLI11](https://github.com/CLIUtils/CLI11) which is licensed under 3-Clause BSD license, [drc-fir](https://drc-fir.sourceforge.net/) which is licensed under GPLv2 and [ALSA.CPP](https://github.com/TheSalarKhan/ALSA.CPP).