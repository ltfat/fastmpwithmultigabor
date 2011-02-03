% MPTK - Matlab interface
% Imports a binary Matching Pursuit anywave file to Matlab, using MPTK
%
% Usage : anywaveTable = anywaveread('filename')
%
% Input : 
% filename : the filename where to read the anywaveTable
%
% Output:
% anywaveTable: a anywaveTable structure with the following structure
%		tableFileName : The 'xml' table associated with the anywave table
%		dataFileName : The 'bin' file containing the wave datas
%		normalized : The flag indicating if the waveforms have been normalized
%		centeredAndDenyquisted: 0
%
% See also : anywavewrite
%
% Authors:
% Boris Mailhé (IRISA, Rennes, France)
% Ronan Le Boulch (IRISA, Rennes, France)
% 
% Distributed under the General Public License.
%                                       
%#mex