% MPTK - Matlab interface
% Decomposes a signal with MPTK
% 
% Usage: [book,residual,decay] = mpdecomp(signal,sampleRate,dict,numIter, ...)
%
% Inputs: 
% signal    	: a numSamples x numChans signal (each column is a channel)
% sampleRate	: the sampleRate of the signal
% dict      	: either a dictionary Matlab strucure, or a filename
% numIter   	: the number of iterations to perform
%
% Other options are specified in pairs with 'flag', value
%   -s: minimum SRR to acheive in pursuit (alternative stopping criterion)
%   -Z: refine only atom amplitudes (default = false)
% 
% specify both if desired
%   -L: number of improvement cycles between augmentations (default = 1)
%   -O: stop cycling when SRR dB improvement is less than (default = 1e-3)
%
% specify only one or the other
%   -K: augment model with K atoms before cycling (default = 1)
%   -J: augment model until J dB improvement before cycling (default = 0)
%
% specify both if desired
%   -M: stop all refinement cycles after M augmentations (default = 1e4)
%   -Q: stop all refinement cycles once SRR dB exceeds (default = 60)
%  
% Outputs:
% book      : the book with the resulting decomposition (warning : its sampleRate is 1 by default)
% residual  : the residual obtained after the iterations
% decay     : a numIter x 1 vector with the energy of the residual after each iteration
%
% Example: [bookCMP, residualCMP, decayCMP] = cmpdecomp(x,Fs,D,100,'-K',2,'-L',2,'-M',20);
%
% 
% See also : sigread, sigwrite, bookread, bookwrite, mprecons, dictread, dictwrite
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
