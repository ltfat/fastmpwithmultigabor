function varargout = mpf_wrap(in1,varargin)

% MPF
% wrapper of mpf binary in matlab.
% -----------------------------------------------------
% Syntax:
% ------
%   [mpf_command_string] = mpf('bookFILE.bin', 'Property1', Val1,...
%                               'Property2', Val2);
%
% Correspondance:
% -X<value> or --X=<value>  => ...,'X',<value>,...
% -X                        => ...,'X',1,...
% if <value> = [min:max]    => ...,'X',[min max],...
% 
% remark:* the numbers are passed as numbers, no need to pass them as
%        strings
%        * optional arguments bookYes.bin, bookNo.bin are converted in
%        (property, value) pairs: properties are 'bookYes','bookNo'.
%
%  Usage (of mpd binary):
%
%      mpf --PROPERTY1=[min:max] ... --PROPERTY_N=[min:max] (bookIn.bin|-) [bookYes.bin|-] [bookNo.bin]
%  
%  Synopsis:
%      Filters the atoms contained in bookIn.bin (or stdin), stores those which satisfy
%      the indicated properties in bookYes.bin (or stdout) and the others in bookNo.bin.
%  
%  Mandatory arguments:
%      (bookIn.bin|-)      A book of input atoms, or stdin.
%  
%  Optional arguments:
%      (bookYes.bin|-)     A file (or stdout) to store the book of atoms which satisfy the indicates properties.
%      bookNo.bin          A file to store a book of atoms which do not satisfy the indicated properties.
%      If no output files are given, the atoms are just counted and their number is reported in stderr.
%  
%      One or more of the following switches:
%      --index=[min:max]    / -i [min:max] : keep the atoms ordered from min to max in the book
%      --length=[min:max]   / -l [min:max] : keep a specific range of atom lengths (in number of samples)
%      --Length=[min:max]   / -L [min:max] : keep a specific range of atom lengths (in seconds)
%      --position=[min:max] / -p [min:max] : keep a specific range of atom positions (in number of samples)
%      --Position=[min:max] / -P [min:max] : keep a specific range of atom positions (in seconds)
%      --freq=[min:max]     / -f [min:max] : keep a specific frequency range (in normalized values between 0 and 0.5)
%      --Freq=[min:max]     / -F [min:max] : keep a specific frequency range (in Hz)
%      --amp=[min:max]      / -a [min:max] : keep a specific range of amplitudes
%      --chirp=[min:max]    / -c [min:max] : keep a specific range of chirp factors
%      The intervals can exclude the min or max value by using reverted braces,
%      e.g. ]min:max] will exclude the min value.
%      The intervals can be negated with prepending the '^' character, e.g. ^[min:max].
%  
%      --type=gabor|harmonic|dirac / -t gabor|harmonic|dirac : test the atom type.
%  
%  Other optional arguments are:
%  
%      -q, --quiet          No text output.
%      -v, --verbose        Verbose.
%      -V, --version        Output the version and exit.
%      -h, --help           This help.
%  
%  Example:
%      Take all the atoms with a frequency lower than 50Hz and higher than 1000Hz
%      among the first 100 atoms of bookIn.bin, store them in bookYes.bin
%      and store all the others in bookNo.bin:
%      mpf --index=[0:100[ --Freq=^[50:1000] bookIn.bin bookYes.bin bookNo.bin
%  
%  Note:
%      Only one instance of each property is allowed. If you want to elaborate more complicated domains,
%      use a pipe.

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

%MPTKinit;

PropStruc = struct(varargin{:});

Mat2MPTKLoadSettings;
PropStruc = Mat2MPTKUpdateOptions(PropStruc, DefPropStruc);

MPTKPath = PropStruc.MPTKPath;

StrBookNo = [];
Strindex = [];
Strlength = [];
StrLength = [];
Strposition = [];
StrPosition = [];
Strfreq = [];
StrFreq = [];
Stramp = [];
Strchirp = [];
Strtype = [];
Strq = [];
Strv = [];
StrV = [];
Strh = [];

if isfield(PropStruc, 'BookYes')
    StrBookYes = [PropStruc.BookYes ' '];

else
    StrBookYes = 'temp.bin';

end


if isfield(PropStruc, 'BookNo')
    StrBookNo = [PropStruc.BookNo ' '];
end


if isfield(PropStruc,'index')
    indexRange = PropStruc.index;
    Strindex = ['-i ' indexRange ' '];
end

if isfield(PropStruc,'i')
    indexRange = PropStruc.i;
    Strindex = ['-i ' indexRange ' '];
end

if isfield(PropStruc,'length')
    lengthRange = PropStruc.length;
    Strlength = ['-l ' lengthRange ' '];
end

if isfield(PropStruc,'l')
    lengthRange = PropStruc.l;
    Strlength = ['-l ' lengthRange ' '];
end

if isfield(PropStruc,'Length')
    LengthRange = PropStruc.Length;
    StrLength = ['-L ' LengthRange ' '];
end

if isfield(PropStruc,'l')
    LengthRange = PropStruc.l;
    StrLength = ['-L ' LengthRange ' '];
end

if isfield(PropStruc,'position')
    positionRange = PropStruc.position;
    Strposition = ['-p ' positionRange ' '];
end

if isfield(PropStruc,'p')
    positionRange = PropStruc.p;
    Strposition = ['-p ' positionRange ' '];
end

if isfield(PropStruc,'Position')
    PositionRange = PropStruc.Position;
    StrPosition = ['-P ' PositionRange ' '];
end

if isfield(PropStruc,'P')
    PositionRange = PropStruc.P;
    StrPosition = ['-P ' PositionRange ' '];
end

if isfield(PropStruc,'freq')
    freqRange = PropStruc.freq;
    Strfreq = ['-f ' freqRange ' '];
end

if isfield(PropStruc,'f')
    freqRange = PropStruc.f;
    Strfreq = ['-f ' freqRange ' '];
end

if isfield(PropStruc,'Freq')
    FreqRange = PropStruc.Freq;
    StrFreq = ['-F ' FreqRange ' '];
end

if isfield(PropStruc,'F')
    FreqRange = PropStruc.F;
    StrFreq = ['-F ' FreqRange ' '];
end

if isfield(PropStruc,'amp')
    ampRange = PropStruc.amp;
    Stramp = ['-a ' ampRange ' '];
end


if isfield(PropStruc,'a')
    ampRange = PropStruc.a;
    Stramp = ['-a ' ampRange ' '];
end



if isfield(PropStruc,'chirp')
    chirpRange = PropStruc.chirp;
    Strchirp = ['-c ' chirpRange ' '];
end

if isfield(PropStruc,'c')
    chirpRange = PropStruc.c;
    Strchirp = ['-c ' chirpRange ' '];
end

if isfield(PropStruc,'type')
    typeRange = PropStruc.type;
    Strtype = ['-t ' typeRange ' '];
end

if isfield(PropStruc,'t')
    typeRange = PropStruc.t;
    Strtype = ['-t ' typeRange ' '];
end


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
STREXEC = [MPTKPath,'mpf ',in1,' ',Strindex,Strlength,StrLength,Strposition,...
StrPosition,Strfreq,StrFreq,Stramp,Strchirp,Strtype,Strq,Strv,StrV,Strh,...
StrBookYes, StrBookNo];

system(STREXEC);
varargout{1} = STREXEC;


