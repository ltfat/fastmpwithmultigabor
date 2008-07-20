% MPTK - Matlab interface
% Export a dictionary description from Matlab to a file, using MPTK
%
% Usage: isvalid = dictwrite(dict[,filename])
%
% Inputs: 
% dict     : a dictionary description with the following structure
%      dict.block{i} = block
%  where, for example
%      block.type = 'dirac'
%  and block may have other field names
%
% filename : the filename where to write the dictionary description in XML
%            if ommited, we just check if the syntax of dict is valid
%
% Outputs: 
% isvalid   : indicates if the dictionary structure was correctly formed. 
%
% See also : dictread
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
