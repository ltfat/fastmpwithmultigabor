% MPTK - Matlab interface
% Decomposes a signal with MPTK
% 
% Usage: [book,residual,decay] = mpdecomp(signal,sampleRate,dict,numIter)
%
% Inputs: 
% signal    : a numSamples x numChans signal (each column is a channel)
% sampleRate: the sampleRate of the signal
% dict      : either a dictionary Matlab strucure, or a filename
% numIter   : the number of iterations to perform
%
% Outputs:
% book      : the book with the resulting decomposition (warning : its sampleRate is 1 by default)
% residual  : the residual obtained after the iterations
% decay     : a numIter x 1 vector with the energy of the residual after each iteration
% 
% See also : sigread, sigwrite, bookread, bookwrite, bookrecons, dictread, dictwrite
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
