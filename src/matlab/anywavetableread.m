% MPTK - Matlab interface
% Imports a binary Matching Pursuit anywave file to Matlab, using MPTK
%
% Usage : anywaveTable = anywaveread('fileTablename')
%
% Input : 
% fileTablename : The 'xml' fileTablename where to read the anywaveTable
%
% Output:
% anywaveTable: A anywaveTable structure with the following structure
%		tableFileName : The 'xml' table associated with the anywave table
%		dataFileName : The 'bin' file containing the wave datas
%		normalized : The flag indicating if the waveforms have been normalized
%		centeredAndDenyquisted : 0
%		wave : 0
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