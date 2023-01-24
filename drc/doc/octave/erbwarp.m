function lambda = erbwarp(fs)
#ERBWARP - Computation of warping coefficient
#
# lambda = erbwarp(fs)
#
# computes an optimal warping coefficient vs sample rate in Hertz
# to approximate the psychoacoustic Erb scale
# approximation by J.O. Smith and J.S. Abel (1995)
#
# If output argument is not specified, e.g.,
# >> barkwarp(44100)
# the function plots the warping function and shows the optimal 
# coefficient value on the title line of the figure. 
# The turning point frequency, i.e., 
# the frequency below which the frequency resolution of the 
# system is higher than in a non-warped system, is also shown in
# the figure as a vertical line.
#
# This function is a part of WarpTB - a Matlab toolbox for
# warped signal processing (http://www.acoustics.hut.fi/software/warp/).
# See 'help WarpTB' for related functions and examples

# Authors: Matti Karjalainen, Aki H�rm�
# Helsinki University of Technology, Laboratory of Acoustics and
# Audio Signal Processing

# Adapted to Octave by Denis Sbragion

# (Smith&Abel, 1999) Formula (30)
lambda = 0.7446*sqrt((2/pi)*atan(0.1418*fs/1000))+0.03237; 

if nargout==0,
  F=freqz([-lambda 1],[1 -lambda]);
  a=-angle(F);
  tp=fs/(2*pi)*atan(sqrt(lambda^(-2)-1));
  grid
  __gnuplot_raw__("set xlabel \"Frequency [kHz]\"\n"); 
  __gnuplot_raw__("set ylabel \"Normalized warped frequency\"\n");
  axis([0 fs/2000 0 1]);
  plot(linspace(0,fs/2000,512),a/pi,sprintf(";ERB warping function for lambda = %f;", lambda));
end

endfunction
