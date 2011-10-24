
% Usage : 
%   book = bookread(filename)
%
% Synopsis:
%   Imports a binary Matching Pursuit book file “filename” to Matlab, using MPTK and returns a structured book “book”.
%
% Input : 
%   * filename : the filename where to read the book
%
% Output:
%   * book : a book structure
%
% Known limitations : 
%   * Only the following atom types are supported :
%       - gabor, harmonic, mdct, mclt, dirac.

%% Authors:
% Gilles Gonon
% Remi Gribonval
% Copyright (C) 2008 IRISA
%
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
