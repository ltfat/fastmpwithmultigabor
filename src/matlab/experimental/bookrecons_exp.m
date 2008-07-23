% MPTK - Matlab interface
% Reconstructs a signal from a book, using MPTK
% 
% Usage: signal = mprecons(book[,residual])
%
% Inputs: 
% book       : a book Matlab structure
% residual   : an optional numSamples x numChans matrix, 
%              which dimensions should match the fields book.numSamples, book.numChans
%
% Outputs:
% signal: the reconstructed signal, a numSamples x numChans matrix
%
% See also : sigread, sigwrite, bookread, bookwrite
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
