function [varargout] = mpd(sig, Fs, varargin)
% MPD
% Matlab version of mpd binary
% -----------------------
% Syntax:
% ------
%        [Book [,residual [,decay]]] = mpd (signal, SampleRate,
%        'Property1', 'Value1', ...)
% - signal: column vector of the signal to analyse
% - SampleRate: sample rate of the signal
% main properties
% -   'D':  Read the dictionary from xml file FILE.
%  
% -   'n','num_iter', 'num_atoms':    Stop after N iterations.
% AND/OR 's', 'snr'              :    Stop when the SNR value SNR is reached.
%                                               If both options are used together, the algorithm stops
%                                               as soon as either one is
%                                               reached.
%
% If this properties are not mentioned the default values are taken (see
% ~/.mptk/MPTKconfig.txt)
% Other properties: type 'help mpd_wrap'

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

wavwrite(sig, Fs, 'tempin.wav');

mpd_wrap('tempin.wav', 'tempbook.bin', 'E', 'tempdecay.txt', varargin{:}, 'tempres.wav');

varargout{1} = bookread('tempbook.bin');
varargout{2} = wavread('tempres.wav');
varargout{3} = ReadDecay('tempdecay.txt');

delete tempin.wav
delete tempbook.bin
delete tempres.wav
delete tempdecay.txt