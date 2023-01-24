## Copyright (C) 2005-2014 Denis Sbragion
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
##(at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## This function is part of DRC - Digital room correction

## usage: m = hdmp(s)
##
## Compute the minimum phase reconstruction of the
## signal s by homomorphic deconvolution.
##

function m = hdmp(s)
	m = abs(fft(s));
	m = ifft(log(m .* (m > 0) + realmin() .* (m <= 0)));
	l = floor(rows(m) / 2) + 1;
	m(2:l,:) *= 2;
	m((l + 1):rows(m),:) = 0;
	m = real(ifft(exp(fft(m))));
endfunction