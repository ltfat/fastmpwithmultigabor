function varargout = mpf(Book, varargin)

% MPF
% Matlab version of mpf binary (filtering function)
% -----------------------------
% Syntax:
% ------
%   [BookYes, [BookNo]] = mpf(Book, 'Property1', Value1');
%
%   - Book: Book to filter
%   - BookYes: Book with the atoms answering to the query(ies)
%   - BookNo:  Book with the atoms not answering to the query(ies)
%
%  type 'help mpf_wrap' for the filtering options

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

bookwrite(Book, 'tempbook.bin');
mpf_wrap('tempbook.bin', 'BookYes', 'tempyes.bin',...
                              'BookNo', 'tempno.bin', varargin{:});
varargout{1} = bookread('tempyes.bin');
varargout{2} = bookread('tempno.bin');
                          
delete tempbook.bin
delete tempyes.bin
delete tempno.bin