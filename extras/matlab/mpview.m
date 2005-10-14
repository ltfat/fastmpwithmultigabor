function mpview( fileName , varargin )

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
fclose(fid);
numChans = size(tf,1)/(numRows*numCols);
tf1 = reshape(tf,numRows,numCols,numChans);
size(tf1)
for c=1:numChans;
    subplot(numChans,1,c);
    if numChans>1
        imagesc(10*log(1+tf1(:,:,i)));
    else
        imagesc(10*log(1+tf1(:,:)));
    end
    colorbar;   
end