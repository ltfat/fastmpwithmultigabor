function [h,gh,hh,dh] = bookover( book, x, chan );

% Usage :
%   bookover(book,signal)
%   bookover(book,signal,chan)
%
% Synopsis :
%   Overlays a book plot on a STFT spectrogram
%
% Detailed description :
%   * bookover(book,signal) plots the book over a STFT spectrogram of the signal using the 
%     default channel (chan : 1).
%   * bookover(book,signal,chan) plots the book over a STFT spectrogram of the signal, for channel 
%     number ÒchanÓ. The book and/or the signal can be given as filenames (WAV format for the signal).
%
% Notes :
%    bookover will resize the current axes to fit the spectrogram area.
%    The patches indicate the locations of the atom supports. The color indicates the energy
% level according to the JET colormap (more energy --> closer to red, less energy --> closer to blue)

%%
%% Authors:
%% Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author: sacha $
%%   $Date: 2006-05-11 20:14:33 +0200 (Thu, 11 May 2006) $
%%   $Revision: 555 $
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
h = surf( T, F, 20*log10(abs(S)) );
shading interp; %shading flat;
%colormap(flipud(gray));

xlabel('time ->');
ylabel('frequency ->');

% Book plot
[gh,hh,dh] = bookplot( book, chan );
%set( gh, 'facecolor', 'r', 'edgecolor', 'none', 'facealpha', 0.4 );
%set( hh, 'facecolor', 'g', 'edgecolor', 'none', 'facealpha', 0.4 );
%set( dh, 'color', 'c', 'alpha', 0.5 );

% Cheat the colormaps
%colormap([flipud(gray(64));hot(64)]);
colormap([flipud(gray(64));jet(64)]);
cax = caxis;
mncaxrng = cax(1);
caxrng = cax(2) - cax(1);
caxis([0 1]);

% - Spectro:
cdat = get( h, 'cdata' );
mncd = min(min(cdat)); mxcd = max(max(cdat)); rngcd = mxcd-mncd;
cdat = (cdat - mncd) / rngcd / 2;
set( h, 'cdata', cdat);

% - Book plot:
cdat = get( gh, 'cdata' );
cdat = [cdat; get( hh, 'cdata' )];
mncd = min(min(cdat)); mxcd = max(max(cdat)); rngcd = mxcd-mncd;

cdat = get( gh, 'cdata' );
cdat = 0.5 + (cdat - mncd) / rngcd / 2;
set( gh, 'cdata', cdat);

cdat = get( hh, 'cdata' );
cdat = 0.5 + (cdat - mncd) / rngcd /2;
set( hh, 'cdata', cdat);

% Cheat the spectrogram's height
SPECTRO_OFFSET_LEVEL = 100;
set( h, 'zdata', get(h,'zdata') - SPECTRO_OFFSET_LEVEL );

set(gca,'xlim',[0 tmax-wl/fs],'ylim',[0 fs/2]);
axis xy; view(2); drawnow;
