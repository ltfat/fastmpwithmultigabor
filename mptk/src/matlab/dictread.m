
% Usage :
%   dict = dictread(filename)
%
% Synopsis :
%   Imports a dictionary description “dict” from a file “filename” to Matlab, using MPTK
%
% Input : 
%   * filename : the filename where to read the dictionary
%
% Output :
%   * dict : a dictionary description
%
% Detailed description :
%   dict is a dictionary description with the following structure dict.block{i} = block 
% where, for example block.type = dirac’ and block may have other field names

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
