function [gaborP,harmP,diracL] = bookplot( book );

% BOOKPLOT  Plot a Matching Pursuit book in the current axes
%
%    BOOKPLOT( book ) plots a book structure in the current axes.
%    If book is a string, it is understood as a filename and
%    the book is read from the corresponding file. Books
%    can be read separately using the BOOKREAD utility.
%
%    [gaborP,harmP,diracP] = BOOKPLOT( book ) returns handles
%    on the created objects. Gabor and Harmonic atoms are
%    plotted as patches, and Dirac atoms as a blue line.
%
%    The patches delimit the support of the atoms. Their
%    color is proportional to the atom's amplitudes,
%    mapped to the current colormap and the current caxis.
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
%% CVS log:
%%   $Author$
%%   $Date$
%%   $Revision$
%%

if isstr(book),
   disp('Loading the book...');
   book = bookread( book );
   disp('Done.');
end;

l = book.numSamples;
fs = book.sampleRate;

gaborX = [];
gaborY = [];
gaborC = [];
harmX = [];
harmY = [];
harmC = [];
diracX = [];
diracY = [];
diracZ = [];

for i = 1:book.numAtoms,

    atom = book.atom{i};

    switch atom.type,

	   case 'gabor',
		p = atom.pos(1)/fs;
		l = atom.len(1)/fs;
		bw2 = ( fs / (atom.len(1)/2) ) / 2;
		A = atom.amp(1); A = 20*log10(A);
		f = fs*atom.freq;

		pv = [p;p;p+l;p+l];
		fv = [f-bw2; f+bw2; f+bw2; f-bw2];

		gaborX = [gaborX,pv];
		gaborY = [gaborY,fv];
		gaborC = [gaborC,A];

	   case 'harmonic',
		p = atom.pos(1)/fs;
		l = atom.len(1)/fs;
		bw2 = ( fs / (atom.len(1)/2 + 1) ) / 2;
		A = atom.amp(1);
		f = atom.freq;

		pv = repmat([p;p;p+l;p+l],1,atom.numPartials);

		fv = fs*atom.freq*atom.harmonicity';
		fvup = fv+bw2;
		fvdown = fv-bw2;
		fv = [fvup;fvdown;fvdown;fvup];

		av = A*atom.partialAmpStorage'; av = 20*log10(av);

		harmX = [harmX,pv];
		harmY = [harmY,fv];
		harmC = [harmC,av];

	   case 'dirac',
		p = atom.pos(1)/fs;
		A = atom.amp(1); A = 20*log10(A);
		diracX = [diracX;NaN;p;p];
		diracY = [diracY;NaN;0;fs/2];
		diracZ = [diracZ;NaN;A;A];

	   % Unknown atom type
	   otherwise,
		error( [ '[' atomType '] is an unknown atom type.'] );
    end;

end;

gaborP = patch( gaborX, gaborY, gaborC, 'edgecol', 'none' );
harmP  = patch( harmX,  harmY,  harmC,  'edgecol', 'none' );
diracL = line( diracX, diracY, diracZ, 'color', 'k' );
