# DRC: Digital Room Correction
#
# Copyright (C) 2002-2019 Denis Sbragion
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# You can contact the author on Internet at the following address:
#
#		d.sbragion@neomerica.it
#
# This program uses the FFT  routines from Takuya  Ooura and the GNU
# Scientific Library (GSL)  FFT routines. Many  thanks to Takuya Ooura and
# the GSL developers for these efficient routines.

# DRC base makefile

# DRC version
VERSION=3.2.3

# Destination directory
DESTDIR=

# Where to install
INSTALL_PREFIX=$(DESTDIR)/usr

# Compiler
CC=g++

# Standard optimization settings, with support for
# default compilation flags
CFLAGS+=-O2 -I.

# Standard optimization settings,
# tested also on Mac OS X
# CFLAGS=-O -s -I.

# Standard optimization settings, for systems missing getopt
# CFLAGS=-O2 -I. -I./getopt

# i686 optimizaztions settings
# CFLAGS=-march=pentium3 -O -mfancy-math-387 -msse -mtune=pentium3 -I.

# Pentium 3 or greater optimizaztions settings with SSE support
# Be careful: -mfpmath=sse sometimes causes accuracy problems
# CFLAGS=-march=pentium3 -O -s -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I.

# Same as above, but tested on recent Mac OS X
# Be careful: -mfpmath=sse sometimes causes accuracy problems
# CFLAGS=-march=pentium3 -O -s -m32 -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I.

# Athlon64 settings
# CFLAGS=-march=athlon64 -O3 -s -fPIC -I.

# Vectorization settings (GCC 4 Only, experimental, only partially working)
# CFLAGS=-march=pentium3 -O3 -s -pg -g -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -ftree-vectorize -ftree-vectorizer-verbose=2 -I.

# Debug settings

# Warning check pedantic settings
# Strong optimization is enabled because some warning are't checked without it
# Remove the optimization options for normal debugging
# CFLAGS=-O3 -pedantic -Wextra -Wall -g -DDebugMLeaks -D_DEBUG_NEW_EMULATE_MALLOC -I. -IMLeaks

# Profiling settings
# CFLAGS=-march=pentium3 -O -pg -g -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I.

# Optimization settings for glsweep
# Some of the strong optimization settings provided above may
# cause accuracy problems with glsweep. If you decide to use them
# it's better to use some specific settings for glsweep, for example like:
#
# GLSCFLAGS=-O2 -I.
#
GLSCFLAGS=$(CFLAGS)

# DRC sources
DRCSRC=baselib.cpp drc.cpp fft.cpp hd.cpp slprefilt.cpp bwprefilt.cpp convol.cpp dspwind.cpp spline.cpp fir.cpp level.cpp toeplitz.cpp kirkebyfd.cpp drccfg.cpp psychoacoustic.cpp cmdline.cpp cfgparse.cpp fftsg.c gsl/gsl_fft.c gsl/error.c gsl/stream.c minIni/minIni.c

# DRC sources for debug and warning checks
# DRCSRC=baselib.cpp drc.cpp fft.cpp hd.cpp slprefilt.cpp bwprefilt.cpp convol.cpp dspwind.cpp spline.cpp fir.cpp level.cpp toeplitz.cpp kirkebyfd.cpp drccfg.cpp psychoacoustic.cpp cmdline.cpp cfgparse.cpp fftsg.c gsl/gsl_fft.c gsl/error.c gsl/stream.c minIni/minIni.c MLeaks/debug_new.cpp

# DRC sources for systems missing getopt
# DRCSRC=baselib.cpp drc.cpp fft.cpp hd.cpp slprefilt.cpp bwprefilt.cpp convol.cpp dspwind.cpp spline.cpp fir.cpp level.cpp toeplitz.cpp kirkebyfd.cpp drccfg.cpp psychoacoustic.cpp cmdline.cpp cfgparse.cpp fftsg.c gsl/gsl_fft.c gsl/error.c gsl/stream.c minIni/minIni.c getopt/getopt.c getopt/getopt1.c

# GLSweep sources
GLSWEEPSRC=glsweep.c

# LSConv sources
LSCONVSRC=lsconv.c fftsg_h.c

# Compiled objects
OBJS=drc lsconv glsweep

# Installation definitions
CONFIG_TARGETS=config/*
TARGET_TARGETS=target/*
MIC_TARGETS=mic/*
DOC_TARGETS=readme.txt
BIN_TARGETS=drc lsconv glsweep

# Targets

ALL: drc glsweep lsconv

drc: $(DRCSRC)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o drc $(DRCSRC) -lm -lstdc++

glsweep: $(GLSWEEPSRC)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(GLSCFLAGS) -o glsweep $(GLSWEEPSRC) -lm

lsconv: $(LSCONVSRC)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o lsconv $(LSCONVSRC) -lm

clean:
	rm -f $(OBJS)

install: $(CONFIG_TARGETS) $(TARGET_TARGETS) $(MIC_TARGETS) $(DOC_TARGETS) $(BIN_TARGETS)
	install -d $(INSTALL_PREFIX)/bin
	install $(BIN_TARGETS) $(INSTALL_PREFIX)/bin
	install -d $(INSTALL_PREFIX)/share/drc/config/
	cp --recursive --no-dereference --preserve=links --no-preserve=ownership $(CONFIG_TARGETS) $(INSTALL_PREFIX)/share/drc/config/
	install -d $(INSTALL_PREFIX)/share/drc/target/
	cp --recursive --no-dereference --preserve=links --no-preserve=ownership $(TARGET_TARGETS) $(INSTALL_PREFIX)/share/drc/target/
	install -d $(INSTALL_PREFIX)/share/drc/mic/
	cp --recursive --no-dereference --preserve=links --no-preserve=ownership $(MIC_TARGETS) $(INSTALL_PREFIX)/share/drc/mic/
	install -d $(INSTALL_PREFIX)/share/doc/drc-$(VERSION)/
	install $(DOC_TARGETS) $(INSTALL_PREFIX)/share/doc/drc-$(VERSION)/
