function mpd_demix_wrap(in1, in2, varargin)
% MPD_DEMIX_WRAPPER
% Wrapper for mpd_demix binary
% ----------------------------
% Syntax:
%   mpd_demix_wrap(sndFILE.wav, bookFILE, 'Property1', 'Value1', ...);
%
%
%  Usage of mpd binary:
%      mpd_demix [options] -D dictFILE.txt -M matrix.txt (-n N|-s SNR) (sndFILE.wav|-) (bookFILE) [residualFILE.wav]
%  
%  Synopsis:
%      Performs Blind Source Separation on signal sndFILE.wav with dictionary dictFile.txt
%      and with the known mixer matrix mixFILE.txt. The result is stored in as many books
%      as estimated sources (plus an optional residual signal), after N iterations
%      or after reaching the signal-to-residual ratio SNR.
%  
%  Mandatory arguments:
%      -M<FILE>, --mix-matrix=<FILE>  Read the mixer matrix from text file FILE.
%                                     The first line of the file should indicate the number of rows
%                                     and the number of columns, and the following lines should give
%                                     space-separated values, with a line break after each row.
%                                     EXAMPLE:
%                                      2 3
%                                      0.92  0.38  0.71
%                                      0.71  0.77  1.85
%  
%      -n<N>, --num-iter=<N>|--num-atoms=<N>    Stop after N iterations.
% AND/OR -s<SNR>, --snr=<SNR>                   Stop when the SNR value SNR is reached.
%                                               If both options are used together, the algorithm stops
%                                               as soon as either one is reached.
%  
%      (sndFILE.wav|-)                          The signal to analyze or stdin (in WAV format).
%      (bookFILE)                               The base name of the files to store the books of atoms_n
%                                               corresponding to the N estimated sources. These N books
%                                               will be named bookFILE_n.bin, n=[0,...,N-1].
%  
%  Optional arguments:
%      -D<FILE>, --dictionary=<FILE>    Read the dictionary from text file FILE.
%                                       If no dictionary is given, a default dictionary is used.
%                                       (Use -v to see the structure of the default dictionary
%                                        reported in the verbose information.)
%      -E<FILE>, --energy-decay=<FILE>  Save the energy decay as doubles to file FILE.
%      -Q<FILE>, --src-sequence=<FILE>  Save the source sequence as unsigned short ints to file FILE.
%      -R<N>,    --report-hit=<N>       Report some progress info (in stderr) every N iterations.
%      -S<N>,    --save-hit=<N>         Save the output files every N iterations.
%      -T<N>,    --snr-hit=<N>          Test the SNR every N iterations only (instead of each iteration).
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
% date: 06/01/2005
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

if isfield(PropStruc, 'M');
    mix_matrix = PropStruc.M;
end

if isfield(PropStruc, 'mix_matrix');
    mix_matrix = PropStruc.mix_matrix;
    
end

StrMixMatrix = ['-M' mix_matrix ' '];

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

%StrsndFILE = in1;
%StrbookFILE = in2;


%OPTIONAL ARGUMENTS

StrDecay = [];
StrRepHit = [];
StrSavHit = [];
StrSnrHit = [];
Strq = [];
Strv = [];
StrV = [];
Strh = []; 
StrDict = [];
StrQ = [];


if isfield(PropStruc, 'D')
    dictionary = PropStruc.D;
    StrDict = ['-D' DicoPath dictionary ' '];
end

if isfield(PropStruc, 'dictionary')
    dictionary = PropStruc.dictionary;
    StrDict = ['-D' DicoPath dictionary ' '];
end

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

if isfield(PropStruc, 'Q')
    SourceSeqfile = PropStruc.Q;
    StrDecay = ['-Q' SourceSeqfile ' '];
end

if isfield(PropStruc, 'src_sequence')
    SourceSeqfile = PropStruc.src_sequence;
    StrQ = ['-Q' SourceSeqfile ' '];
end

varargout{1} = [MPTKPath,'@MPDEMIX_EXECUTABLE@ ',StrDict,StrMixMatrix,StrNit,StrSNR,StrDecay,StrRepHit,...
StrSavHit,StrSnrHit,StrQ, Strq,Strv,StrV,Strh,in1,' ',in2,' ',StrRes];

system(['setenv LD_LIBRARY_PATH @CMAKE_INSTALL_PREFIX@/lib; ', varargout{1} ,' -C @CMAKE_INSTALL_PREFIX@/bin/path.xml']);


