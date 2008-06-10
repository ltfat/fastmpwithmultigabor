function varargout = mpd_demix(sig, Fs, M, varargin)

% MPD_DEMIX
%   Matlab interface for the mpd_demixing program (demixing with a known
%   mixing matrix).
% ----------
% Syntax:
% ------
%    Books = mpd_demix(sig, sr, MixingMatrix, 'Property1', Value1,...)
%
% sig: signal to demix
% sr: sample rate
% MixingMatrix: mixing Nchannel x Nsources, containing the weight of each
% source in each channel.
%
% for the properties, type 'help mpd_demix_wrap'.

% ----------------
% project: MPTK wrapper for Matlab
% author: Pierre Leveau
% date: 06/01/2005
% version: 0.4.1
% ----------------

% Creating mixing matrix file
fid = fopen('tempmix.txt', 'w');
fprintf(fid, '%d %d\n', size(M, 1), size(M, 2));
for m1 = 1:size(M, 1)
    for m2 = 1:size(M, 2)
        fprintf(fid, '%d ', M(m1,m2));     
    end
    fprintf(fid, '\n');
end
fclose(fid);
wavwrite(sig, Fs, 'tempin.wav')

mpd_demix_wrap('tempin.wav', 'tempbook', 'M', 'tempmix.txt',varargin{:});

Books = cell(1,size(M, 2));

for m2 = 1:size(M, 2)
    tempbook = ['tempbook_' num2str(m2-1) '.bin'];
    Books{1,m2} = bookread(tempbook);
    delete(tempbook);
end

delete tempin.wav
delete tempmix.txt
varargout{1} = Books;
    
