function [sig_rec, Fs] = mpr(Book, varargin)

% MPR
%   Matlab version of the mpr binary (reconstruction from a Book)
% ------------
% Syntax:
% ------
%    [sig_rec, sr] = mpr(Book[, residual], 'Property1', 'Value1', ...)
% 
% - Book: Book to reconstruct
% - residual: column vector of the residual
% - sig_rec: reconstructed signal
% - sr: sampling rate of the signal

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

bookwrite(Book, 'tempbook.bin');

if floor(nargin/2) ~= nargin/2
   
    mpr_wrap('tempbook.bin', 'temprec.wav', varargin{:});
else
    wavwrite(varargin{1}, Book.sampleRate, 'tempres.wav');
    if nargin > 2

        VARARG = varargin{2:end};
        mpr_wrap('tempbook.bin', 'temprec.wav', 'tempres.wav', VARARG);
    else

        mpr_wrap('tempbook.bin', 'temprec.wav', 'tempres.wav');
    end


    delete tempres.wav
end


[sig_rec, Fs] = wavread('temprec.wav');
delete tempbook.bin
delete temprec.wav
