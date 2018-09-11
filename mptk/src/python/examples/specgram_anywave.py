#!/usr/bin/env python

# Applying MPTK to spectrograms, by vectorising the spectrograms and the atoms.

# By Dan Stowell Spring 2013.

import numpy as np
import numpy.linalg as npla
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os.path
from scikits.audiolab import Sndfile
from scikits.audiolab import Format
from operator import itemgetter
import copy
import csv
from glob import iglob
import os, errno

import mptk

from specgram_anywave_gmmstuff import *

#######################################################
# USER SETTINGS

basepath = '.'
wavpath = "@MPTK_CHIFFCHAFF_FILENAME@" # you may need to customise this - or feel free to try a different file

fs=44100.0
hop=0.5
framelen=512
fftsize=framelen
specfreqrange=(1000, 8500)   #(3000, 8000)
specfreqbinrange = [int(float(f * fftsize)/fs) for f in specfreqrange]
print "specfreqbinrange: ", specfreqbinrange
bintofreq = fs/fftsize
bintotime = (framelen * hop)/fs

#######################################################

# Ensure MPTK is initialised
mptk.loadconfig('@MPTK_CONFIG_FILENAME@') # NOTE: you may need to customise this path

def coswin(len):
	return np.array([np.sin(x * np.pi / len)**2 for x in range(len)])

#window = np.hamming(framelen)
#window = np.sqrt(np.hamming(framelen))
window = np.sqrt(coswin(framelen))

def stft(x):
	hopsamp = int(hop*framelen)
	spec = np.array([np.fft.fft(window*x[i:i+framelen]) 
				 for i in xrange(0, len(x)-framelen, hopsamp)])
	return spec

def istft(spec):
	hopsamp = int(hop*framelen)
	pcm = np.zeros(hopsamp * (np.shape(spec)[0] - 1) + framelen)
	for whichframe in range(np.shape(spec)[0]):
		fftframe = np.zeros(framelen, dtype=np.complex)
		fftframe[specfreqbinrange[0]:specfreqbinrange[1]] = spec[whichframe,:]
		pcm[hopsamp*whichframe:hopsamp*whichframe+framelen] += np.real(np.fft.ifft(window * fftframe))
	return pcm

def file_to_specgram(path, specgrammode=None):
	if specgrammode==None: # default is to do a "normal" spectrogram right here
		if fftsize != framelen: raise ValueError("this mode requires normal fftsize")
		if not os.path.isfile(path):
			raise ValueError("path %s not found" % path)
		sf = Sndfile(path, "r")
		if sf.channels != 1:
			raise Error("ERROR in spemptk: sound file has multiple channels (%i) - mono audio required." % sf.channels)
		if sf.samplerate != fs:
			raise Error("ERROR in spemptk: wanted srate %g - got %g." % (fs, sf.samplerate))
		chunksize = 4096
		pcm = np.array([])
		while(True):
			try:
				chunk = sf.read_frames(chunksize, dtype=np.float32)
				pcm = np.hstack((pcm, chunk))
			except RuntimeError:
				break
		spec = stft(pcm).T
	else:
		raise ValueError("specgrammode not recognised: %s" % specgrammode)
	spec = spec[specfreqbinrange[0]:specfreqbinrange[1],:]
	mags = abs(spec)
	phasifiers = spec / mags
	if specgrammode==None:
		mags = np.log(mags)
	return (mags, phasifiers)

def specgram_to_file(path, mags, phasifiers, normalise=True, specgrammode=None):
	if specgrammode==None:
		mags = np.exp(mags)
	if normalise:
		mags -= np.min(mags)
	cplx = mags * phasifiers
	pcm = istft(cplx.T)
	if normalise:
		pcm /= np.max(pcm)
	outsf = Sndfile(path, "w", Format('wav'), 1, fs)
	outsf.write_frames(pcm)
	outsf.close()

def freq_of_xcorspecgrambin(index):
	return index*bintofreq + specfreqrange[0]

############################################
# mptk-based syllable finding

def make_mptk_dict(flatgrid, basepath, fname):
	"Turns a flattened probe signal into an XML dictionary that can be used in MPTK, and writes it to disk"
	xml = """<?xml version="1.0" encoding="ISO-8859-1" ?>
<dict>
    <libVersion>0.6.0</libVersion>
    <block>
        <param name="blockOffset" value="" />
        <param name="data" value="%s" />
        <param name="filterLen" value="%i" />
        <param name="numChans" value="1" />
        <param name="numFilters" value="1" />
        <param name="type" value="anywave" />
        <param name="windowShift" value="1" />
</block>
</dict>""" % (mptk.anywave_encode(flatgrid), len(flatgrid))
	fp = open("%s/%s" % (basepath, fname), "wb")
	fp.write(xml)
	fp.close()

def specgram_mptk_onefile(gridinfo, basepath, wavpath, trimat=1500):
	(grid, freqs, times) = gridinfo
	(spec, phasifiers) = file_to_specgram(wavpath)
	if trimat:
		print "Trimming spec to max %i frames" % trimat
		spec = spec[:,:trimat]
	print "specgram shape: %s" % (str(np.shape(spec)))
	print "grid     shape: %s" % (str(np.shape(grid)))
	if True:
		specmin = min(map(min,spec))
		specmax = max(map(max,spec))
		print "specmax, specmin: ", (specmax, specmin)
		spec = (spec - specmin)/(specmax-specmin)

	#xco = xcor2d(grid, spec)
	#(1) pad the tpl in freq domain, with zeros
	padding1 = np.zeros(  ((np.shape(spec)[0]-np.shape(grid)[0])/2, np.shape(grid)[1])  )
	padding2 = np.zeros(  (np.shape(spec)[0]-np.shape(grid)[0]-np.shape(padding1)[0], np.shape(grid)[1])  )
	paddedgrid = np.vstack((padding1, grid, padding2))
	print "paddedgridshape: %s" % (str(np.shape(paddedgrid)))

	# plotting for devt:
	plt.figure()
	fig = plt.subplot(3,6,1);
	plt.imshow(paddedgrid, origin='lower', aspect='auto', interpolation='nearest', cmap=cm.binary)
	plt.savefig("output/pdf/plot_spemptk_paddedgrid.pdf", papertype='A4', format='pdf')
	plt.figure()
	plt.imshow(spec,       origin='lower', aspect='auto', interpolation='nearest', cmap=cm.binary)
	plt.savefig("output/pdf/plot_spemptk_spec.pdf",       papertype='A4', format='pdf')

	#(2) vectorise them both -- NOTE THE ".T" -- so that the vec versions go up the freq axis first
	flatspec = spec.T.flatten()
	print "spec[:,0]: " + str(spec[:,0])
	print "spec[:,0] has shape: " + str(np.shape(spec[:,0]))
	print "flatspec: " + str(flatspec[:10])
	flatgrid = paddedgrid.T.flatten()

	print "flatgrid:"
	flatpos = 0
	for xoff in range(np.shape(paddedgrid)[1]):
		for yoff in range(np.shape(paddedgrid)[0]):
			val = int(round(flatgrid[flatpos] * 1000))
			if val == 0:
				print " ",
			else:
				print str(val),
			flatpos += 1
		print ""

	#######################################################################
	#(3) ask mptk for the peaks
	
	# make our dict
	make_mptk_dict(flatgrid, basepath, "output/dic_py_specgram_mptk.xml")


	# deconstruct -- what is the fs used for? not much, with anywave, I guess
	flatspecfs = fs * hop * np.shape(paddedgrid)[0] / framelen
	print "flatspecfs is %g" % flatspecfs
	flatspec = np.array(flatspec, dtype=np.float32)
	print "flatspec is: " + str(flatspec.dtype)
	# HERE WE GO
	(book, residual) = mptk.decompose(flatspec, "%s/output/dic_py_specgram_mptk.xml" % basepath, flatspecfs, numiters=10, bookpath="%s/output/book_py_specgram_mptk.xml" % basepath)

	print "Amplitudes of atoms received:" + str([atom['amp'][0] for atom in book.atoms])

	# reconstruct
	flatrec = mptk.reconstruct(book, "%s/output/dic_py_specgram_mptk.xml" % basepath)
	print "flatrec: " + str(flatrec)
	print "sum(flatrec): " + str(np.sum(flatrec))
	# back to 2D - note, a bit awkward due to .T
	rec = np.reshape(flatrec, (np.shape(spec)[1], np.shape(spec)[0])).T
	print "rec is shape: " + str(np.shape(rec))
	print "rec is sum: " + str(np.sum(rec))
	# plot the reconstructed thing:
	plt.figure()
	plt.imshow(rec, origin='lower', aspect='auto', interpolation='nearest', cmap=cm.binary)
	plt.savefig("%s/output/pdf/plot_spemptk_recons.pdf" % basepath, papertype='A4', format='pdf')

	# NOTE: this next bit is commented out because the resynth quality is poor, it doesn't really add much.
	# Feel free to uncomment and hack around.
	"""
	# maybe even apply the reconstructed magnitudes back onto the full specgram and resynthesise? (ie masking resynthesis)
	# this would give a good idea of what it's "hearing"...
	# TODO: the resynth quality is not good, probably due to window choice
	resynther = rec * (specmax-specmin) + specmin
	specgram_to_file("output/spemptk_resynth.wav", resynther * 0.1, phasifiers, normalise=True)
	"""


################################################################################################
# Ensure we have folders to write our results to
def mkdir_p(path):
    try:
	   os.makedirs(path)
    except OSError as exc: # Python >2.5
	   if exc.errno == errno.EEXIST:
		  pass
	   else: raise

if __name__ == '__main__':
	mkdir_p("%s/output/pdf" % basepath)

	gmm = makeGMM()
	(grid, freqs, times) = gridevalGMM(gmm, fs, fftsize, framelen, hop)

	print "Building template to plot it"
	plt.figure()
	plt.imshow(grid, origin='lower', aspect='auto', interpolation='nearest', cmap=cm.binary)
	ticksubsample = 10
	examplefontsize='large'
	plt.xticks(range(len(times))[::ticksubsample], [round(t, 2) for t in times[::ticksubsample]], fontsize=examplefontsize)
	plt.yticks(range(len(freqs))[::ticksubsample], [int(round(f, -2)) for f in freqs[::ticksubsample]], fontsize=examplefontsize)
	plt.xlabel("Time (s)", fontsize=examplefontsize)
	plt.ylabel("Freq (Hz)", fontsize=examplefontsize)
	#plt.title("Spectro-temporal template", fontsize='x-small')
	plt.savefig("%s/output/pdf/plot_spemptk_grid.pdf" % basepath, papertype='A4', format='pdf')
	plt.clf() # must do this - helps prevent memory leaks with thousands of Bboxes
	print "Finished plotting grid." # Press enter to continue, and analyse the dataset."

	############################################################################
	print "Now the analysis..."
	specgram_mptk_onefile((grid, freqs, times), basepath=basepath, wavpath=wavpath)

