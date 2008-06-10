function varargout = mpd_wrap(in1,in2,varargin)

% MPD_WRAP
% wrapper of mpd binary in matlab.
% -----------------------------------------------------
% Syntax:
% ------
%   [mpd_command_string] = mpd_wrap('sndFILE.wav', 'bookFILE.bin', 'Property1', Val1,...
%                               'Property2', Val2,...
%                                [,residualFILE.wav]);
%
% Correspondance:
% -X<value> or --X=<value>  => ...,'X',<value>,...
% -X                        => ...,'X',1,...
% - '-' in property names are replaced by '_'
%
% remark: single numbers are passed as numbers, no need to pass them as
% strings
%
%  Usage (of mpd binary):
%      mpd [options] -D dictFILE.xml (-n N|-s SNR) (sndFILE.wav|-) (bookFILE.bin|-) [residualFILE.wav]
%  
%  Synopsis:
%      Iterates Matching Pursuit on signal sndFILE.wav with dictionary dictFile.xml
%      and gives the resulting book bookFILE.bin (and an optional residual signal)
%      after N iterations or after reaching the signal-to-residual ratio SNR.
%  
%  Mandatory arguments:
%      -D<FILE>, --dictionary=<FILE>  Read the dictionary from xml file FILE.
%  
%      -n<N>, --num-iter=<N>|--num-atoms=<N>    Stop after N iterations.
% AND/OR -s<SNR>, --snr=<SNR>                   Stop when the SNR value SNR is reached.
%                                               If both options are used together, the algorithm stops
%                                               as soon as either one is reached.
%  
%      (sndFILE.wav|-)                          The signal to analyze or stdin (in WAV format).
%      (bookFILE.bin|-)                         The file to store the resulting book of atoms, or stdout.
%  
%  Optional arguments:
%      -E<FILE>, --energy-decay=<FILE>  Save the energy decay as doubles to file FILE.
%      -R<N>,    --report-hit=<N>       Report some progress info (in stderr) every N iterations.
%      -S<N>,    --save-hit=<N>         Save the output files every N iterations.
%      -T<N>,    --snr-hit=<N>          Test the SNR every N iterations only (instead of each iteration).
%  
%      -p<double>, --preemp=<double>    Pre-emphasize the input signal with coefficient <double>.
%  
%      residualFILE.wav                The residual signal after subtraction of the atoms.
%  
%      -q, --quiet                    No text output.
%      -v, --verbose                  Verbose.
%      -V, --version                  Output the version and exit.
%      -h, --help                     This help.

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2006
% version: 0.4.1
% ----------------

%MPTKinit;



VARARG = varargin;
NARG = nargin;

    
if NARG/2 ~= floor(NARG/2)
    StrRes = VARARG{end};
    PropStruc = struct(VARARG{1:end-1});
else
    StrRes = [];
    PropStruc = struct(VARARG{:});
end

Mat2MPTKLoadSettings;
PropStruc = Mat2MPTKUpdateOptions(PropStruc, DefPropStruc);

DicoPath = PropStruc.DicoPath;
MPTKPath = PropStruc.MPTKPath;


dictionary = [];
% mandatory arguments


if isfield(PropStruc, 'D')
    dictionary = PropStruc.D;
end
    

if isfield(PropStruc, 'dictionary')
    dictionary = PropStruc.dictionary;
end
StrDict = ['-D' DicoPath dictionary ' '];

StrNit = [];
niteration = -1;

if isfield(PropStruc, 'n')
    niteration = PropStruc.n;
end

if isfield(PropStruc, 'num_iter')
    niteration = PropStruc.num_iter;
end


if isfield(PropStruc, 'num_atom')
    niteration = PropStruc.num_atom;
end
if niteration ~= -1
StrNit = ['-n' num2str(niteration) ' '];
end

StrSNR = [];
SNR = -1;
if isfield(PropStruc, 's')
    SNR = PropStruc.s;
end

if isfield(PropStruc, 'snr')
    SNR = PropStruc.snr;
end
if SNR ~= -1
    StrSNR = ['-s' num2str(SNR) ' '];
end


%optional arguments

StrDecay = [];
StrRepHit = [];
StrSavHit = [];
StrSnrHit = [];
Strq = [];
Strv = [];
StrV = [];
Strh = [];
Strp = [];



if isfield(PropStruc, 'E')
    decayfile = PropStruc.E;
    StrDecay = ['-E' decayfile ' '];
end

if isfield(PropStruc, 'energy_decay')
    decayfile = PropStruc.energy_decay;
    StrDecay = ['-E' decayfile ' '];
end

if isfield(PropStruc, 'R')
    ReportHit = PropStruc.R;
    StrRepHit = ['-R' num2str(ReportHit) ' '];
end

if isfield(PropStruc, 'report_hit')
    ReportHit = PropStruc.report_hit;
    StrRepHit = ['-R' num2str(ReportHit) ' '];
end

if isfield(PropStruc, 'S')
    SaveHit = PropStruc.S;
    StrSavHit = ['-S' num2str(SaveHit) ' '];
end

if isfield(PropStruc, 'save_hit')
    SaveHit = PropStruc.save_hit;
    StrSavHit = ['-S' num2str(SaveHit) ' '];
end

if isfield(PropStruc, 'T')
    SnrHit = PropStruc.T;
    StrSnrHit = ['-T' num2str(SnrHit) ' '];
end

if isfield(PropStruc, 'snr_hit')
    SnrHit = PropStruc.snr_hit;
    StrSnrHit = ['-T' num2str(SnrHit) ' '];
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

if isfield(PropStruc, 'p')
    preemp = PropStruc.p;
    Strp = ['-p' num2str(preemp) ' '];
end

if isfield(PropStruc, 'preemp')
    preemp = PropStruc.preemp;
    Strp = ['-p' num2str(preemp) ' '];
end


varargout{1} = [MPTKPath,'@MP_EXECUTABLE@ ',in1,' ',in2, ' ',StrDict,StrNit,StrSNR,StrDecay,StrRepHit,...
StrSavHit,StrSnrHit,Strq,Strv,StrV,Strh,Strp,StrRes];

system(['setenv LD_LIBRARY_PATH @MPTK_PLUGIN_DIR@; ', varargout{1} ,' -C @MPTK_BINARY_DIR@/bin/path.xml']);

