% MPTK - Matlab interface
% Gets various information on MTPK plugins and configuration into Matlab
% 
% Usage :  info = getmptkinfo() 
% 
% Output :
% info : a structure with the information
%      info.atoms
%      info.blocks
%      info.windows
%      info.path
%
%      where 
%  atoms :
%      atoms.type{}
%  blocks :
%      blocks.type{}
%      blocks.info{}
%      blocks.default{}
%
%  windows :
%      windows.type{}
%      windows.needsOption{}
%
%  path : has different fields taken from the configuration file, for example
%      path.reference
%      path.dll_directory
%      path.fftw_wisdom_file
%
% Author : 
% Remi Gribonval (IRISA, Rennes, France), July 2008
%
% Distributed under the General Public License.
%                                       
%#mex
