#!/usr/bin/env python

# A demonstration of pyMPTK, analysing an exponential sweep which we will synthesise.
# By Dan Stowell, March 2013

# First we import some standard numerical tools for python
import numpy as np
from math import exp, sin, pi
import random
import matplotlib.pyplot as plt
# and things for audio read/write
from scikits.audiolab import Sndfile
from scikits.audiolab import Format
import os.path
# Here we import pyMPTK itself, plus some handy plotting functions
import mptk
import mptkplot

##################################################
# USER SETTINGS - you can change these if you like

# Output folder - by default we'll output to the current working directory
outpath = '.'
# Path to the dictionary file used to analyse
dictpath = "%s/dic_chirp_forexpo.xml" % os.path.dirname(__file__)

# Settings for the synthesised signal
fs = 44100.
basefreq = 100.
exprate = 1.2
sigmul = 0.5
noiselev = 0.05
sigdur = 4
##################################################

# Ensure MPTK is initialised
mptk.loadconfig('@MPTK_CONFIG_FILENAME@') # NOTE: you may need to customise this path

# define a function that helps us generate the exponential frequency curve
def sweep_pos_to_freq(pos):
	return basefreq * exp(exprate * (pos / fs))

# precalculate the positions (in seconds) of each sample we'll generate
timeposses = [pos/fs for pos in range(int(sigdur * fs))]


# plot the freq sweep
plt.plot(timeposses, [sweep_pos_to_freq(pos) for pos in range(int(sigdur * fs))])
plt.title("Frequency trajectory")
plt.savefig("%s/plot_expo_sweep_freqcurve.pdf" % outpath, papertype='A4', format='pdf')
plt.close()

# create signal, with noise
phase = 0.
signal = []
for pos in range(int(sigdur * fs)):
	phase += sweep_pos_to_freq(pos) / fs
	datum = (sin(2. * pi * phase) + random.gauss(0, noiselev)) * sigmul
	signal.append(datum)

# plot signal
plt.figure()
plt.plot(timeposses[::10], signal[::10])  # we're skipping every 10 samples because it's a lot of data...
plt.title("Signal")
plt.savefig("%s/plot_expo_sweep_signal.pdf" % outpath, papertype='A4', format='pdf')
plt.close()

# write signal to disk
outsf = Sndfile("%s/expo_sweep.wav" % outpath, "w", Format('wav'), 1, fs)
outsf.write_frames(np.array(signal))
outsf.close()

####################################################
# HERE is where we decompose the signal through mptk
(book, residual) = mptk.decompose(signal, dictpath, fs, snr=1.0, bookpath="%s/sweep_decomp.xml" % outpath)


# plot the chirp atoms found
plt.figure()
mptkplot.plot_chirped(book, fs)
plt.xlim((0,sigdur))
plt.title("Atoms found")
plt.savefig("%s/plot_expo_atomsfound.pdf" % outpath, papertype='A4', format='pdf')
plt.close()


# we can also reconstruct it 
rec = mptk.reconstruct(book, dictpath)

# you may wish to reconstruct externally, for comparison
# e.g. mpr /tmp/sweep_decomp.xml /tmp/sweep_recons_mpr.wav

# plot recsignal
plt.figure()
plt.plot(timeposses[::10], rec[::10])
plt.title("Reconstructed")
plt.savefig("%s/plot_expo_sweep_recons.pdf" % outpath, papertype='A4', format='pdf')
plt.close()


# plot specgram, for comparison
plt.figure()
plt.specgram(signal, 1024, fs, noverlap=512)
plt.savefig("%s/plot_expo_sweep_specgram.pdf" % outpath, papertype='A4', format='pdf')
plt.close()

print("Finished.")

