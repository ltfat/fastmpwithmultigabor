% function info = plugininfo_exp()
% 
% MPTK - Matlab interface
% Gets information on blocks, atoms and windows from MTPK plugins into Matlab
%
% Usage :  info = plugin_info() 
% 
% Output :
% info : a structure with the information
%      info.atoms.type{}
%      info.blocks.type{}
%      info.blocks.info{}
%      info.blocks.default{}
%      info.blocks.type{}
%      info.windows.type{}
%      info.windows.needsOption{}
%
% Author:
% Remi Gribonval, July 2008
% 
% Distributed under the General Public License.
%                                       
%#mex
