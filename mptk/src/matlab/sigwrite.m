
% Usage :
%   sigwrite(signal,fileName,sampleRate)
%
% Synopsis:
%   Exports a signal of any format supported by libsndfile library from Matlab using 
%   the sampling frequency “sampleRate” and writes it under “filename”.
%
% Inputs :
%   signal    : A numSamples x numChans matrix
%   filename  : The filename where to write the signal
%   sampleRate: The sampling frequency

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
