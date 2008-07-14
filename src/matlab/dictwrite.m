% function [isvalid iswritten] = dictwrite(dict,filename)
%
% MPTK - Matlab interface
% Export a dictionary description from Matlab to a file, using MPTK
%
% Usage : [isvalid iswritten] = dictwrite(dict,filename)
%
% Inputs: 
% dict     : a dictionary description with the following structure
%      dict.block{i} = block
%  where, for example
%      block.type = 'dirac'
%  and block may have other field names
%
% filename : the filename where to write the dictionary description in XML
%
% Outputs: 
% isvalid   : indicates if the dictionary structure was correctly formed. 
% iswritten : indicates if the file writing was successfull
%
% Author : Remi Gribonval, July 2008
% Distributed under the General Public License.
%                                       
%#mex
