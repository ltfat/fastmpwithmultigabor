#!/usr/bin/env python

# A demonstration of pyMPTK, analysing a birdsong recording.
# We will compare chirplet and gabor dictionaries.
# We will also do some manipulation and resynthesis.
#   Look at the plots, and listen to the resyntheses
#   (especially the timestretched versions)
#   and notice the significant differences between gabor and chirp.

# By Dan Stowell, March 2013

# First we import some standard numerical tools for python
import numpy as np
import matplotlib.pyplot as plt
# and things for audio read/write
from scikits.audiolab import Sndfile
from scikits.audiolab import Format
import os.path
import copy

# Here we import pyMPTK itself, plus some handy plotting functions
import mptk
import mptkplot

###################################
# USER SETTINGS:

dictpaths = {
	'chirp': "%s/dic_chirp_forexpo.xml" % os.path.dirname(os.path.realpath(__file__)),
	'gabor': "%s/dic_gabor_forexpo.xml" % os.path.dirname(os.path.realpath(__file__)),
	}

# Output folder - by default we'll output to the current working directory
outdir = '.'

wavpath = "@MPTK_CHIFFCHAFF_FILENAME@" # you may need to customise this

plotmaxfreq = 10000

###################################

# Ensure MPTK is initialised
mptk.loadconfig('@MPTK_CONFIG_FILENAME@') # NOTE: you may need to customise this path

# We define a function that will do our analysis.
# This python file can be used as a module (so you can use this function)
#  or it can be called as a script, in which case the code at the bottom invokes this function for us.
def analysewithchirpdict(wavpath, dictname, dictpath, outdir):
	"Convenience function to analyse an audio file, writing out its book and a nice plot of the chirps"
	wavname = os.path.splitext(os.path.basename(wavpath))[0]

	# load file
	sf = Sndfile(wavpath, "r")
	if sf.channels != 1:
		raise RuntimeError("Soundfile '%s' is not mono" % wavpath)
	signal = sf.read_frames(sf.nframes, dtype=np.float32)
	fs = float(sf.samplerate)
	sf.close()

	sigdur = float(len(signal)) / sf.samplerate

	# decompose signal through mptk
	(book, residual) = mptk.decompose(signal, dictpath, fs, snr=10, bookpath="%s/%s_book.xml" % (outdir, wavname))
	outsf = Sndfile("%s/%s_%s_residual.wav" % (outdir, wavname, dictname), "w", Format('wav'), 1, fs)
	outsf.write_frames(residual)
	outsf.close()

	# reconstruction
	rec = mptk.reconstruct(book, dictpath)
	outsf = Sndfile("%s/%s_%s_recons.wav" % (outdir, wavname, dictname), "w", Format('wav'), 1, fs)
	outsf.write_frames(rec)
	outsf.close()

	# plotting
	fig = plt.figure()
	plt.subplot(2,1,1)
	mptkplot.plot_chirped(book, fs, fig)
	plt.xlim((0,sigdur))
	plt.ylim((0,plotmaxfreq))
	plt.title("Atoms found")

	# plot specgram, for comparison
	plt.subplot(2,1,2)
	plt.specgram(signal, 1024, fs, noverlap=512)
	plt.savefig("%s/plot_%s_%s_chirps.pdf" % (outdir, wavname, dictname), papertype='A4', format='pdf')
	plt.close()

	#############################################################################################
	# now let's try some manipulation of the data... let's manipulate the chirps, and reconstruct

	# pitchshift
	pitchfac = 0.125
	koob = copy.deepcopy(book)
	for atom in koob.atoms:
		atom['freq']  *= pitchfac
		if 'chirp' in atom:
			atom['chirp'] *= pitchfac
	cer = mptk.reconstruct(koob, dictpath)
	outsf = Sndfile("%s/%s_%s_pitchshift.wav" % (outdir, wavname, dictname), "w", Format('wav'), 1, fs)
	outsf.write_frames(cer)
	outsf.close()

	# timestretch
	timestretch = 4.
	koob = copy.deepcopy(book)
	koob.numSamples *= timestretch
	for atom in koob.atoms:
		if 'chirp' in atom:
			atom['chirp'] /= timestretch
		atom['pos'] = [val * timestretch for val in atom['pos']]
		atom['len'] = [val * timestretch for val in atom['len']]
	cer = mptk.reconstruct(koob, dictpath)
	outsf = Sndfile("%s/%s_%s_timestretch.wav" % (outdir, wavname, dictname), "w", Format('wav'), 1, fs)
	outsf.write_frames(cer)
	outsf.close()

	# timestretch with harmonic delays...
	timestretch = 4.
	pitchfacs = [val * 0.5 for val in [1, 1.25, 1.5]]
	delayfacs = [val * fs  for val in [0, 0.2, 0.4]]
	ampfac = 0.9
	koob = copy.deepcopy(book)
	# first we'll timestretch
	koob.numSamples *= timestretch
	for atom in koob.atoms:
		if 'chirp' in atom:
			atom['chirp'] /= timestretch
		atom['pos'] = [val * timestretch for val in atom['pos']]
		atom['len'] = [val * timestretch for val in atom['len']]
	# then add the delays
	newatoms = []
	for atom in koob.atoms:
		for whichfac, pitchfac in enumerate(pitchfacs):
			newatom = copy.deepcopy(atom)
			newatom['freq'] *= pitchfac
			newatom['amp'] = [val * ampfac for val in newatom['amp']]
			newatom['pos'] = [val + delayfacs[whichfac] for val in atom['pos']]
			newatoms.append(newatom)
	koob.atoms = newatoms
	cer = mptk.reconstruct(koob, dictpath)
	outsf = Sndfile("%s/%s_%s_harmonicdelay.wav" % (outdir, wavname, dictname), "w", Format('wav'), 1, fs)
	outsf.write_frames(cer)
	outsf.close()

	return book


#############################################################
# this code is what runs if you call this file from the command-line:
if __name__ == '__main__':
	for (dictname, dictpath) in dictpaths.items():
		analysewithchirpdict(wavpath, dictname, dictpath, outdir)

