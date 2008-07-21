% MPTK - Matlab interface
% Export a signal from Matlab to a WAVE file, using MPTK
% 
% Usage: sigwrite(signal,fileName,sampleRate)
%
% Inputs: 
% signal     : a numSamples x numChans matrix
%
% filename : the filename where to write the signal
%
% sampleRate: the sampling frequency
%
% See also : sigread
%
% Note : sigwrite always writes to the WAV format
%        sigread reads from any format supported by libsndfile
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
