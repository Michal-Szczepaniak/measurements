## Copyright (C) 2017 Denis Sbragion
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

##
## usage: w = acgw(n,a)
##
## Computes an approximate Constrained Gaussian Window
##
## Reference:
##  Sebatian Starosielec and Daniel Haegele
##
##  Discrete-time windows with minimal RMS bandwidth
##  for given RMS temporal width.
##
##  Signal Processing 102, 240 (2014)
##
## n: window length
## a: alpha parameter, default 0.5
##
## Example:
##
##  w = acgw(255,5);
##

function w = acgw(n,a = 2.5)
	c = (n - 1) / 2;
	m = gw(c,n,a) - gw(-0.5,n,a) * (gw(c + n,n,a) + gw(c - n,n,a)) / (gw(-0.5 + n,n,a) + gw(0.5 - n,n,a));
	x = (0:(n-1))';	
	w = (gw(x,n,a) - gw(-0.5,n,a) * (gw(x + n,n,a) + gw(x - n,n,a)) / (gw(-0.5 + n,n,a) + gw(0.5 - n,n,a))) / m;
endfunction

# Internal gaussian function
function w = gw(x,n,a)
	w = exp(-0.5 * (a * (x - (n - 1) / 2) / ((n - 1) / 2)) .^ 2);
endfunction