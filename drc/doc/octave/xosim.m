## Copyright (C) 2008 Denis Sbragion
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
## usage: ir = xosim(fs,lpir,lpl,lpd,lpf,lpn,lpt,lpo,hpir,hpl,hpd,hpf,hpn,hpt,hpo)
##
## Makes an xover simulation on two input impulse responses,
## returns the resulting combined impulse response.
##
## fs: sample rate
## lpir: lowpass impulse response
## lpl: lowpass level
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpn: lowpass xover filter length
## lpt: lowpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## lpo: lowpass xover half order, 0 use a linear phase brickwall, 
##      negative use a minimum phase brickwall.
## hpir: highpass impulse response
## hpl: highpass level
## hpd: highpass delay
## hpf: highpass xover frequency
## hpn: highpass xover filter length
## hpt: highpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## hpo: highpass xover half order, 0 use a linear phase brickwall, 
##      negative use a minimum phase brickwall.
##
## Example:
##
##  ir = lrxosim(44100,ls,1,0,80,4096,0,0,lm,1,0,80,4096,0,0);
##

function ir = xosim(fs,lpir,lpl,lpd,lpf,lpn,lpt,lpo,hpir,hpl,hpd,hpf,hpn,hpt,hpo)
	# Computes the xover filters
	[ lpfir, hpfir ] = mkxo(fs,lpl,lpd,lpf,lpn,lpt,lpo,hpl,hpd,hpf,hpn,hpt,hpo);
	
	# Force the same filter length
	lpfir = postpad(lpfir,max(length(lpfir),length(hpfir)));
	hpfir = postpad(hpfir,max(length(lpfir),length(hpfir)));
	
	# Computes the xover impulse response
	ir = postpad(fftconv(lpfir,lpir) + fftconv(hpfir,hpir),length(lpir));
endfunction