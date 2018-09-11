
% Usage :
%   [signal,sampleRate] = sigread(filename)
%
% Synopsis:
%   Reads an imports signal “filename” of any format supported by libsndfile library 
%   to Matlab and gives a matrix “signal” (numSamples x numChans) and the sampling 
%   frequency of the read signal “sampleRate”.
%
% Input :
%   filename   : The filename where to read the signal
%
% Output:
%   signal     : A matrix numSamples x numChans
%   sampleRate : The sampling frequency of the read signal

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
