function test_mpd_demix( utilsPath );

% TEST_ANYWAVE Tests the utils mpd, mpf and mpr, using synthetic data
%
%    test_anywave( utilsPath )
%
% utilsPath is the path where are stored the utils mpd, mpf, mpr
%
% Calls the function create_synthetic_data to create :
% - 1 monochannel table
% - 1 table with two channels
%
% - 1 monophonic signal
% - 3 stereophonic signals
%
% The signals are sparsely composed of atoms of one of the tables (see
% create_synthetic_data for details). This function show how well the
% generating atoms are retrieved via Matching Pursuit, using the MPTK
% library.
%
% The utils mpd, mpf and mpr are called to respectively decompose the
% signals, filter the atoms and recompose the signals.
%
% A plot shows the difference between the original signal and the
% retrieved signal
%
%%
%% Authors:
%% Sylvain Lesage & Sacha Krstulovic & R�mi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author: sacha $
%%   $Date$
%%   $Revision$
%%

tempPath = '/udd/slesage/MPTK/TEST';

filterLen = [8 8 8 32];
filterShift = 1;
numFilters = [1 1 1 1];
sigLen = 100;
numUsedFilters = 4;
numChans = 2;
numSources = 4;

fprintf('\nCreating temporary synthetic data');
local_create_synthetic_data( tempPath, filterLen, filterShift, numFilters, sigLen, numUsedFilters, numChans, numSources )

numIt = 200;

quiet = ' ';%-q';

load( [tempPath '/ANYWAVE_synthetic_data.mat'] );


% signal 1
fprintf('\nProcessing signal 1');

bookName = [tempPath '/temp_book'];
residualName = [tempPath '/res1.wav'];
matrixName = [tempPath '/matrix1.txt'];
dictName = [tempPath '/dict1.xml'];
sigName = [tempPath '/sig1.wav'];
t_line = sprintf('%smpd_demix %s -R 1 -D %s -M %s -n %d %s %s %s', utilsPath, quiet, dictName, matrixName,numIt, sigName, bookName, residualName);
unix(t_line);
src = cell(numSources,1);
for n=1:numSources,
  srcName = sprintf('%s/src_1_%0.2i.wav',tempPath,n);
  t_line = sprintf('%smpr %s %s %s', utilsPath, quiet, sprintf('%s_%0.2i.bin',bookName,n-1), srcName );
  unix(t_line);
  src{n} = wavread(srcName);
end

sigRes = wavread(residualName);

unix(sprintf('rm -f %s',matrixName));

figure(1);
subplot(numSources,3,1);
plot(sig1');
title(sprintf('Original signal 1 (generated by %lu atoms)',numUsedFilters));
subplot(numSources,3,4);
plot(sigRes);
title('Residual of the signal');

for n=1:numSources,
  subplot(numSources,3,2+3*(n-1));
  plot(sig1(n,:)');
  title(sprintf('source %i of the original signal',n));
  subplot(numSources,3,3+3*(n-1));
  plot(src{n});
  title(sprintf('source %i : estimation',n));
end

fprintf(' - Press a key');
drawnow;
pause;

% signal 2
fprintf('\nProcessing signal 2');

bookName = [tempPath '/temp_book'];
residualName = [tempPath '/res2.wav'];
matrixName = [tempPath '/matrix2.txt'];
dictName = [tempPath '/dict_list.xml'];
sigName = [tempPath '/sig2.wav'];
t_line = sprintf('%smpd_demix %s -R 1 -D %s -M %s -n %d %s %s %s', utilsPath, quiet, dictName, matrixName,numIt, sigName, bookName, residualName);
unix(t_line);
src = cell(numSources,1);
for n=1:numSources,
  srcName = sprintf('%s/src_2_%0.2i.wav',tempPath,n);
  t_line = sprintf('%smpr %s %s %s', utilsPath, quiet, sprintf('%s_%0.2i.bin',bookName,n-1), srcName );
  unix(t_line);
  src{n} = wavread(srcName);
end

sigRes = wavread(residualName);

unix(sprintf('rm -f %s',matrixName));

figure(1);
subplot(numSources,3,1);
plot(sig2');
title(sprintf('Original signal 2 (generated by %lu atoms)',numUsedFilters));
subplot(numSources,3,4);
plot(sigRes);
title('Residual of the signal');

for n=1:numSources,
  subplot(numSources,3,2+3*(n-1));
  plot(sig2(n,:)');
  title(sprintf('source %i of the original signal',n));
  subplot(numSources,3,3+3*(n-1));
  plot(src{n});
  title(sprintf('source %i : estimation',n));
end

fprintf(' - Press a key');
drawnow;
pause;

% signal 3
fprintf('\nProcessing signal 3');

bookName = [tempPath '/temp_book'];
residualName = [tempPath '/res3.wav'];
matrixName = [tempPath '/matrix3.txt'];
dictName = [tempPath '/dict_list.xml'];
sigName = [tempPath '/sig3.wav'];
t_line = sprintf('%smpd_demix %s -R 1 -D %s -M %s -n %d %s %s %s', utilsPath, quiet, dictName, matrixName,numIt, sigName, bookName, residualName);
unix(t_line);
src = cell(numSources,1);
for n=1:numSources,
  srcName = sprintf('%s/src_3_%0.2i.wav',tempPath,n);
  t_line = sprintf('%smpr %s %s %s', utilsPath, quiet, sprintf('%s_%0.2i.bin',bookName,n-1), srcName );
  unix(t_line);
  src{n} = wavread(srcName);
end

sigRes = wavread(residualName);

unix(sprintf('rm -f %s',matrixName));

figure(1);
subplot(numSources,3,1);
plot(sig3');
title(sprintf('Original signal 3 (generated by %lu atoms)',numUsedFilters));
subplot(numSources,3,4);
plot(sigRes);
title('Residual of the signal');

for n=1:numSources,
  subplot(numSources,3,2+3*(n-1));
  plot(src3{n}');
  title(sprintf('source %i of the original signal',n));
  subplot(numSources,3,3+3*(n-1));
  plot(src{n});
  title(sprintf('source %i : estimation',n));
end


fprintf('\n');

return;

function local_create_synthetic_data( savePath, filterLen, filterShift, numFilters, sigLen, numUsedFilters, numChans, numSources )

% CREATE_SYNTHETIC_DATA create 2 dictionaries and 4 synthetic signals with the atoms
%
%    create_synthetic_data( savePath, filterLen, filterShift, numFilters, sigLen, numUsedFilters )
%
%   save the workspace, the tables, the signals (wav at 16 khZ) and the dictionaries to the path given in savePath
%
% filterLen      : length of the atoms (size : 1 x numSources)
% filterShift    : shift between the atoms in the dictionary 
%                  (filterShift == 1 means that the filters are tested at every position)
% numFilters     : number of atoms in the dictionaries (size : 1 x numSources)
% sigLen         : length of the signals created
% numUsedFilters : number of filters used to create the synthetic signals
%                  set it to a little value to generate sparse signals
% numChans       : number of channels in the mixture
% numSources     : number of sources
%
%
%%
%% Authors:
%% Sylvain Lesage & Sacha Krstulovic & R�mi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author: sacha $
%%   $Date$
%%   $Revision$
%%

% create the dictionaries

dicts = cell(1,numSources);

if (length(filterLen) == 1)
  filterLen = filterLen * ones(1,numSources);
end
if (length(numFilters) == 1)
  numFilters = numFilters * ones(1,numSources);
end


for n=1:numSources,
  
  dict.numFilters = numFilters(n);
  dict.numChans = 1;
  dict.filterLen = filterLen(n);
  for filterIdx=1:dict.numFilters
    tempWave = randn(1,dict.filterLen);
    dict.filters(filterIdx).chans(1).wave = tempWave/sqrt(sum(tempWave*tempWave'));
  end
  dicts{n} = dict;
end
clear dict;

% create the matrices 

M1 = eye(numSources);
M2 = eye(numSources);
M3 = randn(numChans,numSources);
norms = sqrt(sum(M3.^2,1));
M3 = M3 ./ norms(ones(numChans,1),:);

% create the signals

% signal 1

sig1 = zeros(numSources,sigLen);
dict = dicts{1};

sig1FiltIdx = randint(1,numUsedFilters,[1 dict.numFilters]);
sig1FiltChanIdx = randint(1,numUsedFilters,[1 numSources]);
sig1FiltLoc = randint(1,numUsedFilters,[1 (sigLen-dict.filterLen+1)]);
sig1FiltAmpl = randn(1,numUsedFilters);
for n = 1:numUsedFilters
  chan = sig1FiltChanIdx(n);
  pos = sig1FiltLoc(n)+(0:dict.filterLen-1);
  sig1(chan,pos) = sig1(chan,pos) + dict.filters(sig1FiltIdx(n)).chans(1).wave * sig1FiltAmpl(n);
end
sig1Factor = 0.95/max(abs(sig1(:)));
sig1 = sig1 * sig1Factor;


% signal 2

sig2 = zeros(numSources,sigLen);

sig2FiltSrcIdx = randint(1,numUsedFilters,[1 numSources]);
sig2FiltIdx = zeros(1,numUsedFilters);
sig2FiltLoc = randint(1,numUsedFilters,[1 (sigLen-max(filterLen(:))+1)]);
sig2FiltAmpl = randn(1,numUsedFilters);
for n = 1:numUsedFilters
  chan = sig2FiltSrcIdx(n);
  dict = dicts{chan};
  pos = sig2FiltLoc(n)+(0:dict.filterLen-1);
  sig2FiltIdx(n) = randint(1,1,[1 dict.numFilters]);
  sig2(chan,pos) = sig2(chan,pos) + dict.filters(sig2FiltIdx(n)).chans(1).wave * sig2FiltAmpl(n);
end
sig2Factor = 0.95/max(abs(sig2(:)));
sig2 = sig2 * sig2Factor;


% signal 3

sig3 = zeros(numChans,sigLen);
src3 = cell(1,numSources);
for n=1:numSources,
  src3{n} = zeros(1,sigLen);
end

sig3FiltSrcIdx = randint(1,numUsedFilters,[1 numSources]);
sig3FiltIdx = zeros(1,numUsedFilters);
sig3FiltLoc = randint(1,numUsedFilters,[1 (sigLen-max(filterLen)+1)]);
sig3FiltAmpl = randn(2,numUsedFilters);
for n = 1:numUsedFilters
  src = sig3FiltSrcIdx(n);
  dict = dicts{src};
  pos = sig2FiltLoc(n)+(0:dict.filterLen-1);
  sig3FiltIdx(n) = randint(1,1,[1 dict.numFilters]);
  sig3(:,pos) = sig3(:,pos) + M3(:,src) * dict.filters(sig3FiltIdx(n)).chans(1).wave * sig3FiltAmpl(n);
  src3{src}(pos) = src3{src}(pos) + dict.filters(sig3FiltIdx(n)).chans(1).wave * sig3FiltAmpl(n);
end
sig3Factor = 0.95/max(abs(sig3(:)));
sig3 = sig3 * sig3Factor;
for n=1:numSources,
  src3{n}=src3{n} * sig3Factor;
end

% saves

samplingFreq = 16000;
bitNum = 16;

% workspace
save([savePath '/ANYWAVE_synthetic_data.mat']);

% signals
wavwrite(sig1',samplingFreq, bitNum, [savePath '/sig1.wav']);
wavwrite(sig2',samplingFreq, bitNum, [savePath '/sig2.wav']);
wavwrite(sig3',samplingFreq, bitNum, [savePath '/sig3.wav']);

% tables and dictionaries
lis = sprintf('%s/dict_list.xml',savePath);
fidList = fopen(lis,'wt');
fprintf(fidList,'%i dictionaries\n',numSources);
for n=1:numSources,
  tab = sprintf('%s/table%i.bin',savePath,n);
  dat = sprintf('%s/table%i_data.bin',savePath,n);
  dic = sprintf('%s/dict%i.xml',savePath,n);
  fprintf(fidList,'%s\n',dic);
  savetable( dicts{n}, tab, dat);

  % dictionaries
  fid = fopen( dic, 'wt');

  fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
  fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

  fprintf(fid, '<dict>\n');
  fprintf(fid, '\t<block type="anywave">\n');
  fprintf(fid, '\t\t<par type="tableFileName">%s</par>\n', tab);
  fprintf(fid, '\t\t<par type="windowShift">%i</par>\n', filterShift);
  fprintf(fid, '\t</block>\n');
  fprintf(fid, '</dict>');

  fclose( fid );
  
end
fclose(fidList);

% mixing matrices
matrix1 = [savePath '/matrix1.txt'];
matrix2 = [savePath '/matrix2.txt'];
matrix3 = [savePath '/matrix3.txt'];

fid1 = fopen(matrix1,'wt');
fid2 = fopen(matrix2,'wt');
fid3 = fopen(matrix3,'wt');

fprintf(fid1,'%i %i',numSources,numSources);
fprintf(fid2,'%i %i',numSources,numSources);
fprintf(fid3,'%i %i',numChans,numSources);

for n=1:numSources,
  fprintf(fid1,'\n');
  fprintf(fid2,'\n');
  fprintf(fid3,'\n');
  for m=1:numSources,
    fprintf(fid1,'%i ',M1(m,n));
    fprintf(fid2,'%i ',M2(m,n));
  end
  for m=1:numChans,
    fprintf(fid3,'%i ',M3(m,n));
  end
end

fclose(fid1);
fclose(fid2);
fclose(fid3);
