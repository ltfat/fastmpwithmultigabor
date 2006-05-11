function [gaborP,harmP,diracL] = bookplot( book, channel, bwfactor );

% BOOKPLOT  Plot a Matching Pursuit book in the current axes
%
%    BOOKPLOT( book, chan ) plots the channel number chan
%    of a MPTK book structure in the current axes.
%    If book is a string, it is understood as a filename and
%    the book is read from the corresponding file. Books
%    can be read separately using the BOOKREAD utility.
%
%    BOOKPLOT( book ) defaults to the first channel.
%
%    [gaborP,harmP,diracP] = BOOKPLOT( book, chan ) returns
%    handles on the created objects. Gabor and Harmonic atoms
%    are plotted as patches, and Dirac atoms as a blue line.
%
%    The patches delimit the support of the atoms. Their
%    color is proportional to the atom's amplitudes,
%    mapped to the current colormap and the current caxis.
%
%    BOOKPLOT( book, chan, bwfactor ) allows to specify
%    the bandwidths of the atoms, calculated as:
%      bw = ( fs / (atom.length(channel)/2) ) / bwfactor;
%    where fs is the signal sample frequency. When omitted,
%    bwfactor defaults to 2.
%
%    See also BOOKREAD, BOOKOVER, COLORMAP, CAXIS and
%    the patch handle graphics properties.

%%
%% Authors:
%% Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author$
%%   $Date$
%%   $Revision$
%%

if isstr(book),
   disp('Loading the book...');
   book = bookread( book );
   disp('Done.');
end;

if nargin < 2,
   channel = 1;
end;

if channel > book.numChans,
   error('Book has %d channels. Can''t display channel number %d.', ...
	       channel, book.numChans );
end;

if nargin < 3,
   bwfactor = 2;
end;

l = book.numSamples;
fs = book.sampleRate;

gaborX = [];
gaborY = [];
gaborZ = [];
gaborC = [];
harmX = [];
harmY = [];
harmZ = [];
harmC = [];
diracX = [];
diracY = [];
diracZ = [];

for i = 1:book.numAtoms,

    atom = book.atom{i};

    switch atom.type,

	   case 'gabor',
		p = atom.pos(channel)/fs;
		l = atom.len(channel)/fs;
		bw2 = ( fs / (atom.len(channel)/2) ) / bwfactor;
		A = atom.amp(channel); A = 20*log10(A);
		f = fs*atom.freq;
		c = fs*fs*atom.chirp;

		pv = [p;p;p+l;p+l];
		fv = [f-bw2; f+bw2; f+bw2+c*l; f-bw2+c*l];
		av = [A; A; A; A];

		gaborX = [gaborX,pv];
		gaborY = [gaborY,fv];
		gaborZ = [gaborZ,av];
		gaborC = [gaborC,A];

	   case 'harmonic',
		p = atom.pos(channel)/fs;
		l = atom.len(channel)/fs;
		bw2 = ( fs / (atom.len(channel)/2 + 1) ) / bwfactor;
		A = atom.amp(channel);
		f = atom.freq;
		c = fs*fs*atom.chirp;

		pv = repmat([p;p;p+l;p+l],1,atom.numPartials);

		fv = fs*atom.freq*atom.harmonicity';
		dfv = c*l;
		fvup = fv+bw2;
		fvdown = fv-bw2;
		fv = [fvup;fvdown;fvdown+dfv;fvup+dfv];

		cv = A*atom.partialAmpStorage(:,channel)';
		cv = 20*log10(cv);

		av = [cv;cv;cv;cv];

		harmX = [harmX,pv];
		harmY = [harmY,fv];
		harmZ = [harmZ,av];
		harmC = [harmC,cv];

	   case 'dirac',
		p = atom.pos(channel)/fs;
		A = atom.amp(channel); A = 20*log10(A);
		diracX = [diracX;NaN;p;p];
		diracY = [diracY;NaN;0;fs/2];
		diracZ = [diracZ;NaN;A;A];

	   % Unknown atom type
	   otherwise,
		error( [ '[' atomType '] is an unknown atom type.'] );
    end;

end;

gaborP = patch( gaborX, gaborY, gaborZ, gaborC, 'edgecol', 'none' );
harmP  = patch( harmX,  harmY,  harmZ,  harmC,  'edgecol', 'none' );
diracL = line( diracX, diracY, diracZ, 'color', 'k' );
