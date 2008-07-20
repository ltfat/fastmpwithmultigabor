% MPTK - Matlab interface
% Imports a dictionary description from a file to Matlab, using MPTK
% 
% Usage: dict = dictread(filename)
%
% Input: 
% filename : the filename where to read the dictionary description in XML
%
% Output:
% dict     : a dictionary description with the following structure
%      dict.block{i} = block
%  where, for example
%      block.type = 'dirac'
%  and block may have other field names
%
% See also : dictwrite
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
