#!/usr/bin/env python

# this file is about building a GMM and evaluting it to create the template
# which is custom for matching against certain birdsong sounds

import numpy as np
import numpy.linalg as npla

class GaussianComponent:
	"""Represents a single Gaussian component, 
	with a float weight, vector location, matrix covariance."""
	def __init__(self, weight, loc, cov):
		self.weight = float(weight)
		self.loc    = np.array(loc, dtype=float, ndmin=2)
		self.cov    = np.array(cov, dtype=float, ndmin=2)
		self.loc    = np.reshape(self.loc, (np.size(self.loc), 1)) # enforce column vec
		self.cov    = np.reshape(self.cov, (np.size(self.loc), np.size(self.loc))) # ensure shape matches loc shape
		# precalculated values for evaluating gaussian:
		k = len(self.loc)
		self.part1 = (2.0 * np.pi) ** (-k * 0.5)
		self.part2 = np.power(npla.det(self.cov), -0.5)
		self.invcov = np.linalg.inv(self.cov)

	def __str__(self):
		return "GaussianComponent(%g, %s, %s)" % (self.weight, str(self.loc), str(self.cov))

	def valueat(self, x):
		x = np.array(x, dtype=float)
		x = np.reshape(x, (np.size(self.loc), 1)) # enforce column vec
		dev = x - self.loc
		#print "dev is ", dev
		part3 = np.exp(-0.5 * np.dot(np.dot(dev.T, self.invcov), dev))
		return self.part1 * self.part2 * part3 * self.weight

tplfreqrange = (3100, 7500)  #(4500, 7500)   # overall freq range for the grid-evaluated gmm
tplknee = (0.12, 4900)    # the location of the beginning of the hoizontal bar in the template

def makeGMM(numsper = [20, 20]):
	"gives 2D GMM"
	comps = []
	eachweight = 1. / sum(numsper)
	# freq std was 100 for both, and timestd 0.01
	for freqfrm,	freqtoo,    timefrm,    timetoo, numper,	freqstd, timestd in [
	    [7000,	  5000,	  0.085,	 0.11,    numsper[0], 100,	0.01,    ], 
	    [tplknee[1], tplknee[1], tplknee[0], 0.19,    numsper[1], 100,	0.01,    ]]:
		for i in xrange(numper):
			b = float(i)/numper
			a = 1. - b
			freq = freqfrm * a + freqtoo * b
			time = timefrm * a + timetoo * b
			g = GaussianComponent(eachweight, [freq, time		 ], [[freqstd ** 2, 0],    [0, timestd ** 2]])
			#print g
			comps.append(g)
	return comps

def gridevalGMM(gmm, fs, fftsize, framelen, hop, timefrm=0.05, timetoo=0.2, freqstep=None, timestep=None, norm=True):
	freqstep = freqstep or (fs/fftsize)
	timestep = timestep or (hop*framelen/fs)
	freqs = np.arange(tplfreqrange[0], tplfreqrange[1], freqstep)
	times = np.arange(timefrm, timetoo, timestep)

	vals = np.zeros((len(freqs), len(times)))
	tot = 0.
	for fi, freq in enumerate(freqs):
		for ti, time in enumerate(times):
			for g in gmm:
				val= g.valueat([freq, time])
				tot += val
				vals[fi,ti] += val
	if norm:
		vals *= (1./tot)
	return (vals, freqs, times)

