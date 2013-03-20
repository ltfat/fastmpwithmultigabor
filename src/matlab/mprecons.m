
% Usage :
%   signal = mprecons(book, dict[,residual])
%
% Synopsis :
%   Reconstruct a signal “signal” from a “book”. An optional residual can be added, 
%   under the condition that its dimension matches the fields “book.numSamples” 
%   and “book.numChans”.
%
% Inputs : 
%   book     : a book Matlab structure
%   dict     : a dict Matlab structure
%   residual : OPTIONAL a numSamples x numChans matrix, which dimensions should 
%              match the fields book.numSamples, book.numChans
%
% Outputs :
%   signal   : the reconstructed signal, a numSamples x numChans matrix

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
