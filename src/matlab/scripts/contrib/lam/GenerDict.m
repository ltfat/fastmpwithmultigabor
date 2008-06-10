function DicoFile = GenerDict(DicoFile, varargin)

% GENERDICT
% Generates a standard MPTK dictionary file (fixed fft size, window shift,
% window type). Scales can be spanned linearly or exponentially. If
% generated, harmonic atoms have the same parameters as the Gabor ones.
% 
% ----------------------
% Syntax:
%   DicoFilePath = GenerDict(DicoName, 'Option1', OptionValue1, ...);
% 
% Options:
% - Path: path where the dictionary file is stored.
% - minScale: minimum scale in samples (def: 512)
% - maxScale: maximum scale in samples (def: 4096)
% - DiracAtoms: 'On': extract Dirac atoms (def: 'Off')
% - HarmoAtoms: 'On': extract harmonic Atoms with the same parameters as the
%  corresponding Gabor atoms (def: 'Off')
% - fftSize: FFT size (def: 4096)
% - WinType: fft window type (def: 'hamming')
% - WinOpt: window option
% - windowShift: windowShift (def: 128)
% - Scaling: 
%     *'exp': scales are computed exponentially (e.g. 256, 512, 1024...)
%     *'lin': scales are computed linearly (e.g. 256, 512, 768, 1024...)
% (def: 'exp')
% - ScaleStep: *for linear scaling, step between two consecutive scales (def:
% windowShift)
%              *for exponential scaling, factor between two conscutive scales
% (def: 2)
% - f0Min: for harmonic atoms, minimum fundamental frequency in Hz(def: 40);
% - f0Max: """""""""""""""""", maximum """""""""""""""""""""""""""(def: 3150);
% - numPartials: """""""""""", number of partials (def: 15);
% NB: these three last options set 'HarmoAtoms' on.



% %%
%% Author:
%% Pierre Leveau
% 22/01/06
% ver 0.4.1 rev 3


% SET PARAMETERS
PropStruc = struct(varargin{:});
Mat2MPTKLoadSettings;
PropStruc = Mat2MPTKUpdateOptions(PropStruc, DefPropStruc);

if isfield(PropStruc,'DicoPath')
    DicoPath = PropStruc.DicoPath;
end

if isfield(PropStruc,'minScale')
    minScale = PropStruc.minScale;
else
    minScale = 512;
end

if isfield(PropStruc,'maxScale')
    maxScale = PropStruc.maxScale;
else
    maxScale = 4096;
end

if isfield(PropStruc,'HarmoAtoms')
    HarmoAtoms = PropStruc.HarmoAtoms;
else
    HarmoAtoms = 'Off';
end

if isfield(PropStruc,'ChirpAtoms')
    ChirpAtoms = PropStruc.ChirpAtoms;
else
    ChirpAtoms = 'Off';
end

if isfield(PropStruc,'fftSize')
    fftSize = PropStruc.fftSize;
else
    fftSize = max([4096 maxScale]);
end

if isfield(PropStruc,'windowShift')
    windowShift = PropStruc.windowShift;
else
    windowShift = 128;
end

if isfield(PropStruc, 'Scaling')
    Scaling = PropStruc.Scaling;
else
    Scaling = 'exp';
end

if strcmp(Scaling, 'exp')
    ScaleStep = 2;
else
    ScaleStep = windowShift;
end

if isfield(PropStruc, 'ScaleStep')    
    ScaleStep = PropStruc.ScaleStep;
end
    

if isfield(PropStruc, 'DiracAtoms')
    DiracAtoms = PropStruc.DiracAtoms;
else
    DiracAtoms = 'Off';
end

if isfield(PropStruc, 'f0Min')
    f0Min = PropStruc.f0Min;
   % HarmoAtoms = 'On';
else
    f0Min = 200;
end

if isfield(PropStruc, 'f0Max')
    f0Max = PropStruc.f0Max;
    %HarmoAtoms = 'On';
else
    f0Max = 3150;
end

if isfield(PropStruc, 'numPartials')
    numPartials = PropStruc.numPartials;
   % HarmoAtoms = 'On';
else
    numPartials = 15;
end

if isfield(PropStruc, 'WinType')
    WinType = PropStruc.WinType;
    
else
    WinType = 'hamming';
end

if isfield(PropStruc, 'WinOpt')
    WinOpt = PropStruc.WinOpt;
    
else
    WinOpt = '0';
end

if isfield(PropStruc, 'numIter')
    numIter = PropStruc.numIter;
    
else
    numIter = 1;
end

if isfield(PropStruc, 'numFitPoints')
    numFitPoints = PropStruc.numFitPoints;
    
else
    numFitPoints = 3;
end


switch Scaling
    case 'exp'
        Scales = 2.^(round(log2(minScale)):log2(ScaleStep):round(log2(maxScale)));
    case 'linear'
        Scales = minScale:ScaleStep:maxScale;
    otherwise
        error('Wrong scale type');
end

%WRITE FILE

DicoFilePath = fullfile(DicoPath, DicoFile);
fid = fopen(DicoFilePath, 'w');
fprintf(fid,'<?xml version="1.0" encoding="ISO-8859-1" ?>\n');
fprintf(fid, '<dict>\n');
fprintf(fid, '<libVersion>0.5.4</libVersion>\n');
fprintf(fid, '\t<blockproperties name="HAMMING-WINDOW">\n');
fprintf(fid, '\t\t<param name="windowShift" value="%d" />\n', windowShift);
fprintf(fid, '\t\t<param name="windowtype" value="%s" />\n', WinType);
fprintf(fid, '\t\t<param name="windowopt" value="%s" />\n', WinOpt);
fprintf(fid, '\t\t<param name="fftSize" value="%d" />\n', fftSize);
fprintf(fid, '\t</blockproperties>\n');	

if ~strcmpi(HarmoAtoms, 'only') && ~strcmpi(ChirpAtoms, 'only')
for sc = Scales
fprintf(fid, '\t<block uses="HAMMING-WINDOW">\n');
fprintf(fid, '\t\t<param name="type" value="gabor"/>\n');
fprintf(fid, '\t\t<param name="windowLen" value="%d" /> \n',sc);			
fprintf(fid, '\t</block>\n');
end
end

if ~strcmpi(HarmoAtoms, 'off')
fprintf(fid, '\t<blockproperties name="HAMMING-WINDOW-FO" refines="HAMMING-WINDOW">\n');
fprintf(fid, '\t\t<param name="f0Min" value="%d" />\n',f0Min);
fprintf(fid, '\t\t<param name="f0Max" value="%d" />\n',f0Max);
fprintf(fid, '\t\t<param name="numPartials" value="%d" />\n',numPartials);
fprintf(fid, '\t</blockproperties>\n');
    
    for sc = Scales
fprintf(fid, '\t<block uses="HAMMING-WINDOW-FO">\n');
fprintf(fid, '\t\t<param name="type" value="harmonic"/>\n');
fprintf(fid, '\t\t<param name="windowLen" value="%d" /> \n',sc);			
fprintf(fid, '\t</block>\n');
    end
end

if ~strcmpi(ChirpAtoms, 'off')
fprintf(fid, '\t<blockproperties name="HAMMING-WINDOW-CHIRP" refines="HAMMING-WINDOW">\n');
fprintf(fid, '\t\t<param name="numIter" value="%d" />\n',numIter);
fprintf(fid, '\t\t<param name="numFitPoints" value="%d" />\n',numFitPoints);
fprintf(fid, '\t</blockproperties>\n');

    for sc = Scales
        
fprintf(fid, '\t<block uses="HAMMING-WINDOW-CHIRP">\n');
fprintf(fid, '\t\t<param name="type" value="chirp"/>\n');
fprintf(fid, '\t\t<param name="windowLen" value="%d" /> \n',sc);			
fprintf(fid, '\t</block>\n');            
    end
end

if strcmpi(DiracAtoms, 'on')
    fprintf(fid, '\t<block>\n'); 
    fprintf(fid, '\t\t<param name="type" value="dirac"/>\n');
    fprintf(fid, '\t</block>\n'); 
end

fprintf(fid, '</dict>');
fclose(fid);
    
