function varargout = mpcat(varargin)


%MPCAT
% Matlab interface for the mpcat program (concatenates Books)
% ---------------
% Syntax:
% ------
%   BookOut = mpcat(BookIn1, BookIn2..., BookInN, 'Property1', Value1,...)
%   
% BookIn1...: Books to concatenate (minimum 2)
% BookOut: concatenated book
%
%   property list: type 'help mpcat_wrap'

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

k=1;
BookCell = cell(1,1);
while k <= nargin && isstruct(varargin{k}) 
    BookCell{k} = ['temp' num2str(k) '.bin'];
    bookwrite(varargin{k}, BookCell{k});
    
    k = k+1;
end

mpcat_wrap(k-1,BookCell{:},'tempout.bin' ,varargin{k:end});
varargout{1} = bookread('tempout.bin');

for l = 1:k-1
    delete(['temp' num2str(l) '.bin']);
end
delete tempout.bin

