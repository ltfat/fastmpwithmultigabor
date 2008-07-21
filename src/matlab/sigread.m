% MPTK - Matlab interface
% Imports a signal from a file to Matlab, using MPTK
% 
% Usage: [signal,sampleRate] = sigread(filename)
%
% Input: 
% filename : the filename where to read the signal
%
% Output:
% signal     : a matrix numSamples x numChans
%
% sampleRate : the sampling frequency of the read signal
%
% See also : sigwrite
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
