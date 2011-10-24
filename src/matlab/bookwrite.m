
% Usage : 
%   bookwrite(book,filename[,writeMode]) 
%
% Synopsis:
%   Exports a binary Matching Pursuit book “book” from Matlab, and writes it either in binary format or in txt format under the directory “filename”
%
% Input : 
%   * book : A book structure
%   * filename : The filename where to read the book
%   * writeMode : OPTIONAL The book write mode ('binary' by default or 'txt')
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
