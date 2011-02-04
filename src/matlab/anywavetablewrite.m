% MPTK - Matlab interface
% Imports a binary Matching Pursuit book file to Matlab, using MPTK
%
% Usage : anywavewrite(anywaveTable,'fileTablename', 'fileDatasname')
%
% Input : 
% anywaveTable: A anywaveTable structure with the following structure
%		tableFileName : The 'xml' table associated with the anywave table
%		dataFileName : The 'bin' file containing the wave datas
%		normalized : The flag indicating if the waveforms have been normalized
%		centeredAndDenyquisted : 0
%		wave : 0
% fileTablename : The 'xml' fileTablename where to write the anywaveTable
% fileDatasname : the 'bin' fileDatasname where to write the anywave wave datas
%
% See also : anywaveread
%
% Authors:
% Ronan Le Boulch (IRISA, Rennes, France)
% 
% Distributed under the General Public License.
%                                       
%#mex