function tf = mpview( fileName , varargin )

% MPVIEW Displays a time-frequency plot of a Matching Pursuit book
%        obtained using the command mpview from the Matching Pursuit Toolkit
%
% Example : 
%    in a shell : mpview book.bin tfmap.flt
%    in matlab  : MPVIEW( 'tfmap.flt' ) 
%
%    See also BOOKPLOT, BOOKOVER.

%%
%% Authors:
%% Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author: sacha $
%%   $Date: 2005-07-25 22:33:12 +0200 (Mon, 25 Jul 2005) $
%%   $Revision: 25 $
%%

numCols = 640;
numRows = 480;

if nargin>1, numCols = varargin{1}; end;
if nargin>2, numRows = varargin{2}; end;

fid = fopen(fileName,'r');
tf = fread(fid,'float');
%tf = fread(fid,'ushort');
fclose(fid);

numChans = size(tf,1)/(numRows*numCols);
tfmax = max(tf);
tf = 10*log10(max(tf,tfmax/1e3));
tf = reshape( tf, numRows, numCols, numChans );
size( tf )

if numChans == 1,
   imagesc( tf );
   colorbar;   
else,
   for c = 1:numChans;
       subplot(numChans,1,c);
       imagesc( tf(:,:,c) );
       colorbar;   
   end;
end;
