function [h,gh,hh,dh] = bookover( book, x, chan );

% BOOKOVER Overlays a book plot on a STFT spectrogram
%
%    BOOKOVER( book, sig, chan ) plots the given book over
%    a STFT spectrogram of the given signal, for channel
%    number chan.
%    The book and/or the signal can be given as filenames
%    (WAV format for the signal).
%
%    BOOKOVER( book, sig ) defaults the channel to 1.
%
%    [sh,gh,hh,dh] = BOOKOVER( book, sig, chan ) return handles
%    on the created objects:
%       sh => spectrogram surf
%       gh => gabor atoms patch    (green patch)
%       hh => harmonic atoms patch (red patch)
%       dh => dirac atoms line     (cyan line)
%    The patches indicate the locations of the atom supports.
%
%    Notes:
%    - The colors are NOT proportional to the amplitudes.
%      (Use BOOKPLOT instead.)
%    - BOOKOVER will resize the current axes to fit
%      the spectrogram area.
%
%    See also BOOKPLOT, BOOKREAD.

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

if nargin < 3,
   chan = 1;
end;

if isstr(book),
   disp('Loading the book...');
   book = bookread( book );
   disp('Done.');
end;

if chan > book.numChans,
   error('Book has %d channels. Can''t display channel number %d.', ...
	       chan, book.numChans );
end;

if isstr( x ),
   disp('Loading the signal...');
   [x,fs] = wavread( x );
   disp('Done.');
   if (fs ~= book.sampleRate),
      warning('The signal sample frequency and the book sample frequency differ.');
   end;
end;

nSigChans = size(x,2);
if chan > nSigChans,
   error('Signal has %d channels. Can''t display channel numer %d.', ...
	       chan, nSigChans );
end;

if ~exist('fs'),
   fs = book.sampleRate;
end;

x=x(:,chan);
l = size(x,1);
t = (1:l)' / fs;
tmax = l/fs;

% Spectrogram
wl = 2^nextpow2( 0.025*fs );
[S,F,T] = specgram(x,wl,fs,hamming(wl),wl/2);
%imagesc(20*log10(abs(S)));
h = surf( T, F, 20*log10(abs(S)) - 30 );
shading flat;
colormap(flipud(gray));

xlabel('time ->');
ylabel('frequency ->');

% Book plot
[gh,hh,dh] = bookplot( book, chan );
set( gh, 'facecolor', 'r', 'edgecolor', 'none', 'facealpha', 0.4 );
set( hh, 'facecolor', 'g', 'edgecolor', 'none', 'facealpha', 0.4 );
set( dh, 'color', 'c', 'alpha', 0.5 );

set(gca,'xlim',[0 tmax-wl/fs],'ylim',[0 fs/2]);
axis xy; view(2); drawnow;
