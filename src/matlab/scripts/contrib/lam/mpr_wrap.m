function varargout = mpr_wrap(in1,in2,varargin)

% MPR_WRAP
% wrapper of mpr binary in matlab.
% -----------------------------------------------------
% Syntax:
% ------
%   [mpr_command_string] = mpr_wrap('bookFILE.bin', 'reconstructed.wav', 'Property1', Val1,...
%                               'Property2', Val2);
%
% Correspondance:
% -X<value> or --X=<value>  => ...,'X',<value>,...
% -X                        => ...,'X',1,...
% - '-' in property names are replaced by '_'
%
% remark:* the numbers are passed as numbers, no need to pass them as
%        strings
% -----------------------------------------------------
%
%  Usage (of mpr binary):
% 
%      mpr [options] (bookFILE.bin|-) (reconsFILE.wav|-) [residualFILE.wav]
%  
%  Synopsis:
%      Rebuilds a signal reconsFile.wav from the atoms contained in the book file bookFile.bin.
%      An optional residual can be added.
%  
%  Mandatory arguments:
%      (bookFILE.bin|-)     A book of atoms, or stdin.
%      (reconsFILE.wav|-)   A file to store the rebuilt signal, or stdout (in WAV format).
%  
%  Optional arguments:
%      residualFILE.wav     A residual signal that was obtained from a Matching Pursuit decomposition.
%  
%      -d, --deemp          De-emphasize the signal.
%  
%      -q, --quiet          No text output.
%      -v, --verbose        Verbose.
%      -V, --version        Output the version and exit.
%      -h, --help           This help.

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2006
% version: 0.4.1
% ----------------


%MPTKinit;
Strbook = [in1 ' '];
Strrecons= [in2 ' '];
Strresidual= [];
Strq = [];
Strv = [];
StrV = [];
Strh = [];
Strd = [];

if nargin/2 ~= floor(nargin/2)
    Strresidual = [varargin{1} ' '];
    PropStruc = struct(varargin{2:end});
else
    PropStruc = varargin;
end

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

if isfield(PropStruc, 'd')
    deemp = PropStruc.d;
    Strd = ['-d' num2str(deemp) ' '];
end

if isfield(PropStruc, 'deemp')
    deemp = PropStruc.deemp;
    Strd = ['-d' num2str(deemp) ' '];
end


STREXEC = [MPTKPath,'mpr ',Strbook, Strrecons,...
Strresidual,Strq,Strv,StrV,Strh,Strd];

system(STREXEC);
varargout{1} = STREXEC;
