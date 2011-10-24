
% Usage: 
%   isvalid = dictwrite(dict[,filename])
%
% Synopsis :
%   Exports a dictionary description “dict” from Matlab to a file “filename”, using MPTK. It
% returns “isvalid”, a boolean describing if the syntax of dict is correctly formed
%
% Input : 
%   * dict : a dictionary description
%   * filename : OPTIONAL the filename where to read the dictionary
%
% Output :
%    isvalid : indicates if the dictionary structure was correctly formed. 
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
