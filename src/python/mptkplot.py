# pyMPTK plot functions
# Written by Dan Stowell, March 2013

import matplotlib.pyplot as plt
import matplotlib.cm as cm

def __andmatcher(a,b):
	return a and b

def plot_chirped(book, fs, fig=None, channel=0, linestyle='b-', **kwargs):
	"""Given a 'book' in pyMPTK format, and a samplerate, this plots every chirped-gabor atom in the book.
	(For each atom, it only plots it if the chirped-gabor parameters are present.)
	Amplitude is reflected in the 'alpha' of each line drawn.
	Atoms without the required parameters are silently ignored, so this can be used on mixed books.
	"""

	maxamp = max([atom['amp'][channel] for atom in book.atoms])
	fs = float(fs)

	ownfig = (fig==None)
	if ownfig:
		fig = plt.figure()

	for atom in book.atoms:
		if reduce(__andmatcher, [k in atom for k in ['wintype', 'chirp', 'len', 'freq', 'amp', 'pos']]):
			timedelta = (atom['len'][channel] * 0.5)/fs
			xs = [float(atom['pos'][channel])/fs, atom['pos'][channel]/fs + timedelta + timedelta]
			freq      = atom['freq'] * fs
			freqdelta = atom['chirp'] * fs / timedelta
			ys = [freq, freq + freqdelta + freqdelta]
			plt.plot(xs, ys, linestyle, alpha=atom['amp'][channel]/maxamp, hold=True, **kwargs)

	return fig



