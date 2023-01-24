/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002-2004 Denis Sbragion

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

****************************************************************************/

#ifndef ROOM_CORRECTION_IMPULSERESPONSECONVERTER_H
#define ROOM_CORRECTION_IMPULSERESPONSECONVERTER_H

#include <vector>

class ImpulseResponseConverter {
public:
    static std::vector<float> convert(const std::vector<float>& recording, std::vector<float> inverseSweep);

private:
    static double getRMSLevel(const float *sig, const int sigLength);
    static void hcconvolve(float *a, const float *b, const int n);
};


#endif //ROOM_CORRECTION_IMPULSERESPONSECONVERTER_H
