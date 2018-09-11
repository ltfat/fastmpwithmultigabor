
% Usage : 
%   [book,residual,decay] = mpdecomp(signal,sampleRate,dict,numIter)
%
% Synopsis :
%   Decompose a signal “signal” using its sampling frequency “sampleRate”, a dictionary
% structure “dict” performing “numIter” iterations and gives the resulting decomposition 
% “book”, the “residual” obtained after the iterations and “decay”, a vector with the 
% energy of the residual after each iteration
%
% Inputs: 
%   signal    : a numSamples x numChans signal (each column is a channel)
%   sampleRate: the sampleRate of the signal
%   dict      : either a dictionary Matlab strucure, or a filename
%   numIter   : the number of iterations to perform
%
% Outputs:
%   book      : the book with the resulting decomposition (warning : its sampleRate is 1 by default)
%   residual  : the residual obtained after the iterations
%   decay     : a numIter x 1 vector with the energy of the residual after each iteration

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
