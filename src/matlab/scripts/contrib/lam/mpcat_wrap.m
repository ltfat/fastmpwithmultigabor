function varargout = mpcat_wrap(Nbooks,varargin)

% MPCAT
% wrapper of mpcat binary in matlab.
% -----------------------------------------------------
% Syntax:
% ------
%   [mpcat_command_string] = mpcat(Nbooks_in, 'book1.bin', 'book2.bin', 'bookN.bin', 'bookOut.bin');
% 
%
%  Usage (of mpcat binary):
%
%      mpcat (book1.bin|-) (book2.bin|-) ... (bookN.bin|-) (bookOut.bin|-)
%  
%  Synopsis:
%      Concatenates the N books book1.bin...bookN.bin into the book file bookOut.bin.
%  
%  Mandatory arguments:
%      (bookN.bin|-)        At least 2 books (or stdin) to concatenate.
%      (bookOut.bin|-)      A book where to store the concatenated books, or stdout
%  
%  Optional arguments:
%      -f, --force          Force the overwriting of bookOut.bin.
%  
%      -q, --quiet          No text output.
%      -v, --verbose        Verbose.
%      -V, --version        Output the version and exit.
%      -h, --help           This help.

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 12/01/2005
% version: 0.4.1.1
% ----------------

%MPTKinit;

Strq = [];
Strv = [];
StrV = [];
Strh = [];



    
if nargin > Nbooks+2
    
PropStruc = struct(varargin{Nbooks+2:end});

Mat2MPTKLoadSettings;
PropStruc = Mat2MPTKUpdateOptions(PropStruc, DefPropStruc);


MPTKPath = PropStruc.MPTKPath;



if isfield(PropStruc, 'q')
    qon = PropStruc.q;
    if qon
        Strq = '-q ';
    end
end

if isfield(PropStruc, 'quiet')
    qon = PropStruc.quiet;
    if qon
        Strq = '-q ';
    end
end

if isfield(PropStruc, 'v')
    von = PropStruc.v;
    if von
        Strv = '-v ';
    end
end

if isfield(PropStruc, 'verbose')
    von = PropStruc.verbose;
    if von
        Strv = '-v ';
    end
end

if isfield(PropStruc, 'V')
    Von = PropStruc.V;
    if Von
        StrV = '-V ';
    end
end

if isfield(PropStruc, 'version')
    Von = PropStruc.version;
    if Von
        StrV = '-V ';
    end
end

if isfield(PropStruc, 'h')
    hon = PropStruc.h;
    if hon
        Strh = '-h ';
    end
end

if isfield(PropStruc, 'help')
    hon = PropStruc.help;
    if hon
        Strh = '-h ';
    end
end
else

    Mat2MPTKLoadSettings;


MPTKPath = DefPropStruc.MPTKPath;
end

STREXEC = [MPTKPath 'mpcat '];

for k = 1:Nbooks+1 %all books in and the book out
    STREXEC = [STREXEC varargin{k} ' '];
end

STREXEC = [STREXEC,Strq,Strv,StrV,Strh];

system(STREXEC);

varargout{1} = STREXEC;

