
%   Usage :  
%     info = getmptkinfo() 
%
% Synopsis :
%     Getting started example under Matlab to help using MPTK
%
% Output :
%     The structure “info” has the following informations
%         * info.atoms(x,1), x is the position of the atom
%             - type             The type of the atom (Anywave, Dirac...)
%         * info.blocks(x,1).parameters, x is the position of the block 
%             - name            The name of the block parameter (windowShift, blockOffset...)
%             - type            The type of the block parameter (ulong, real...)
%             - info            Details about the block parameter
%             - default         Details about the block parameter
%         * info.windows(x,1), x is the position of the info
%             - type            The name of the window (hamming, flattop...)
%             - needsOption     A boolean describing if otions are needed
%         * info.path			Includes different path names used by MPTK. Here are the most important :
%             - dll_directory   The path of all required libraries (MPTK plugins, tinyxml, getopt...)
%             - reference		The path of all the reference files for testing
%             - tmp				The path of a temporary folder
%             - fftw_wisdomfile The full path of the fftw_wisdomfile
%             - exampleBook		The full path of a book example
%             - exampleSignal	The full path of a signal example
%

%% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon (IRISA, Rennes, France)
% Remi Gribonval (IRISA, Rennes, France)
%
% Copyright (C) 2008 IRISA
% This script is part of the Matching Pursuit Library package,
% distributed under the General Public License.
%
