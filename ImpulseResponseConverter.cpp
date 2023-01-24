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

#include "ImpulseResponseConverter.h"

#include <cstdio>
#include <cmath>
#include "drc/fftsg_h.h"

double ImpulseResponseConverter::getRMSLevel(const float *sig, const int sigLength) {
    float RMS;
    int i;

    RMS = 0;
    for (i = 0; i < sigLength; i++)
        RMS += sig[i] * sig[i];

    return (double) sqrt(RMS);
}

/* Halfcomplex array convolution */
void ImpulseResponseConverter::hcconvolve(float *a, const float *b, const int n)
{
    float t1;
    float t2;

    a[0] *= b[0];
    a[1] *= b[1];
    for (int r = 2, i = 3; r < n; r += 2, i += 2)
    {
        t1 = a[r] * b[r];
        t2 = a[i] * b[i];
        a[i] = ((a[r] + a[i]) * (b[r] + b[i])) - (t1 + t2);
        a[r] = t1 - t2;
    }
}

std::vector<float> ImpulseResponseConverter::convert(const std::vector<float>& recording, std::vector<float> inverseSweep) {
    /* Convolution parameters */
    int SS;
    int IS;
    int RS;
    int CS;
    int CL;
    int i;
    float *sweep;
    float *inverse;

    /* Computes the convolution length */
    SS = recording.size();
    IS = inverseSweep.size();
    RS = 1;
    CL = SS + IS + RS - 2;
    for(CS = 1;CS < CL;CS <<= 1);

    /* Convolution arrays allocation */
    if ((sweep = (float *) malloc(sizeof(float) * CS)) == NULL) {
        printf("Memory allocation failure.");
        return {};
    }

    if ((inverse = (float *) malloc(sizeof(float) * CS)) == NULL) {
        printf("Memory allocation failure.");
        free(sweep);
        return {};
    }

    for (int j = 0; j < inverseSweep.size(); j++)
        inverse[j] = inverseSweep[j];

    for (i = IS;i < CS;i++)
        inverse[i] = (float) 0.0;

//    printf("inverse filter FFT...\n");
    rdft(CS,OouraRForward,inverse);

    for (int j = 0; j < recording.size(); j++)
        sweep[j] = recording[j];

    for (i = SS;i < CS;i++)
        sweep[i] = (float) 0.0;

//    printf("sweep and inverse convolution...\n");
    rdft(CS,OouraRForward,sweep);
    hcconvolve(sweep,inverse,CS);

    /* Impulse response recover */
    rdft(CS,OouraRBackward,sweep);
    for (i = 0;i < CS;i++)
        sweep[i] *= (float) (2.0  / CS);

    std::vector<float> result;
    for(i = 0;i < CL;i++)
        result.push_back(sweep[i]);

    /* Memory deallocation */
    free(sweep);
    free(inverse);

    return result;
}
