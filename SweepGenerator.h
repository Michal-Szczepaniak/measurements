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

#ifndef ROOM_CORRECTION_SWEEPGENERATOR_H
#define ROOM_CORRECTION_SWEEPGENERATOR_H

#include <vector>

class SweepGenerator {
public:
    std::pair<std::vector<float>, std::vector<float>> generate(
        int rate,
        float amplitude,
        int hzStart,
        int hzEnd,
        int duration,
        int silenceDuration,
        float leadIn,
        float leadOut
    );

private:
    std::vector<float> generateSilence(int length);
};


#endif //ROOM_CORRECTION_SWEEPGENERATOR_H
