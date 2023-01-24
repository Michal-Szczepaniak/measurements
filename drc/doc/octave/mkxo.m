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
## usage: [lp, hp] = mkxo(fs,lpl,lpd,lpf,lpn,lpt,lpo,hpl,hpd,hpf,hpn,hpt,hpo)
##
## Makes a FIR xover pair
##
## fs: sample rate
## lpl: lowpass level
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpn: lowpass xover filter length
## lpt: lowpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## lpo: lowpass xover half order, 0 use a linear phase brickwall, 
##      negative use a minimum phase brickwall.
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
##  [lp, hp] = mkxo(44100,1,0,80,4096,0,0,1,0,80,4096,0,0);
##

function [lp, hp] = mkxo(fs,lpl,lpd,lpf,lpn,lpt,lpo,hpl,hpd,hpf,hpn,hpt,hpo)
	# Compute the filter centers
	if (lpo == 0)
		lpc = (lpn + 1) / 2;
	else
		lpc = 0;
	endif
	if (hpo == 0)
		hpc = (hpn + 1) / 2;
	else
		hpc = 0;
	endif

	# Computes the alignement delays
	if (lpc > hpc)
		lps = 0;
		hps = round(abs(lpc - hpc));
	else
		lps = round(abs(lpc - hpc));
		hps = 0;
	endif
	
	# Computes the low pass filter
	if (lpo > 0)
		# LR24 filter
		[lpb, lpa] = butter(lpo,2 * lpf / fs);
		if (lpt == 0)
			lp = lpl * prepad(filter(lpb,lpa,filter(lpb,lpa,[ 1; zeros(lpn - 1,1) ])) .* flipud(blackman(2 * lpn)(1:lpn)),lps + lpd + lpn);
		else
			lp = lpl * prepad(filter(lpb,lpa,filter(lpb,lpa,[ 1; zeros(lpn - 1,1) ])) .* flipud(chebwin(2 * lpn,lpt)(1:lpn)),lps + lpd + lpn);
		endif
	else
		if (lpo < 0)
			# Minimum phase filter						
			if (lpt == 0)
				lp = bandpassfir(lpn,0,2 * lpf / fs)' .* blackman(lpn);
			else
				lp = bandpassfir(lpn,0,2 * lpf / fs)' .* chebwin(lpn,lpt);
			endif
			lp = hdmp(postpad(lp,32 * lpn))(1:lpn);
			lp = lpl * prepad(lp .* flipud(blackman(2 * lpn)(1:lpn)),lps + lpd + lpn);
		else
			# Linear phase filter
			if (lpt == 0)
				lp = lpl * prepad(bandpassfir(lpn,0,2 * lpf / fs)' .* blackman(lpn),lps + lpd + lpn);
			else
				lp = lpl * prepad(bandpassfir(lpn,0,2 * lpf / fs)' .* chebwin(lpn,lpt),lps + lpd + lpn);
			endif
		endif
	endif
		
	# Computes the high pass filter
	if (hpo > 0)
		# LR24 filter
		[hpb, hpa] = butter(hpo,2 * hpf / fs,'high');
		if (hpt == 0)
			hp = hpl * prepad(filter(hpb,hpa,filter(hpb,hpa,[ 1; zeros(hpn - 1,1) ])) .* flipud(blackman(2 * hpn)(1:hpn)),hps + hpd + hpn);
		else
			hp = hpl * prepad(filter(hpb,hpa,filter(hpb,hpa,[ 1; zeros(hpn - 1,1) ])) .* flipud(chebwin(2 * hpn,hpt)(1:hpn)),hps + hpd + hpn);
		endif		
	else
		if (hpo < 0)
			# Minimum phase filter
			if (hpt == 0)
				hp = bandpassfir(hpn,2 * hpf / fs,1)' .* blackman(hpn);
			else
				hp = bandpassfir(hpn,2 * hpf / fs,1)' .* chebwin(hpn,hpt);
			endif
			hp = hdmp(postpad(hp,32 * hpn))(1:hpn);
			hp = hpl * prepad(hp .* flipud(blackman(2 * hpn)(1:hpn)),hps + hpd + hpn);
		else
			# Linear phase filter
			if (hpt == 0)
				hp = hpl * prepad(bandpassfir(hpn,2 * hpf / fs,1)' .* blackman(hpn),hps + hpd + hpn);
			else
				hp = hpl * prepad(bandpassfir(hpn,2 * hpf / fs,1)' .* chebwin(hpn,hpt),hps + hpd + hpn);
			endif
		endif
	endif
endfunction