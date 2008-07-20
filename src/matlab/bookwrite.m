% MPTK - Matlab interface
% Exports a binary Matching Pursuit book from Matlab, using MPTK
%
% Usage : bookwrite(book,filename[,writeMode]) 
%
% Input : 
% book     : a book structure with the following structure
%    TODO
% filename : the filename where to read the book
%
% Known limitations : only the following atom types are supported: 
%    gabor, harmonic, mdct, mclt, dirac.
%
% See also : bookread bookedit_exp
%
% Authors:
% Emmanuel Ravelli (LAM, Paris, France)
% Gilles Gonon, Remi Gribonval (IRISA, Rennes, France)
% 
% Distributed under the General Public License.
%                                       
%#mex