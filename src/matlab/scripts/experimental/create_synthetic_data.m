function create_synthetic_data( savePath, filterLen, filterShift, numFilters, sigLen, numUsedFilters )

% CREATE_SYNTHETICC_DATA create 2 dictionaries and 4 synthetic signals with the atoms
%
%    create_synthetic_data( savePath, filterLen, filterShift, numFilters, sigLen, numUsedFilters )
%
%   save the workspace, the tables, the signals (wav at 16 khZ) and the dictionaries to the path given in savePath
%
% filterLen      : length of the atoms
% filterShift    : shift between the atoms in the dictionary 
%                  (filterShift == 1 means that the filters are tested at every position)
% numFilters     : number of atoms in the dictionary
% sigLen         : length of the signals created
% numUsedFilters : number of filters used to create the synthetic signals
%                  set it to a little value to generate sparse signals
% 
%
% table 1 :
%   - numFilters normalized filters 
%   - 1 channel
%   - size of the filters : filterLen 
%
% table 2 :
%   - numFilters normalized filters
%   - 2 channels
%   - size of the filters : filterLen
%
% dict 1 :
%   - uses table 1
%   - filterShift = 1
%
% dict 2 :
%   - uses table 2
%   - filterShift = 1
%
% signal 1 :
%   - 1 channel
%   - length = sigLen
%   - composed of numUsedFilters filters randomly taken in dict 1, with random
%     amplitude and random location
%
% signal 2 :
%   - 2 channels
%   - length = sigLen
%   - composed of numUsedFilters filters randomly taken in dict 1, with random
%     amplitude, and random location, each of which being only on
%     one of the two channels.
%
% signal 3 :
%   - 2 channels
%   - length = sigLen
%   - composed of numUsedFilters filters randomly taken in dict 1, with random
%     amplitude, and random location, each of which being present
%     at the same place on the two channels, with different
%     amplitudes
%
% signal 4 :
%   - 2 channels
%   - length = sigLen
%   - composed of numUsedFilters filters randomly taken in dict 2, with random
%     amplitude, and random location
%
%%
%% Authors:
%% Sylvain Lesage & Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author: sacha $
%%   $Date: 2007-07-13 16:20:18 +0200 (Fri, 13 Jul 2007) $
%%   $Revision: 1108 $
%%

numChansMono = 1;
numChansStereo = 2;

% create the dictionaries

dict1.numFilters = numFilters;
dict1.numChans = numChansMono;
dict1.filterLen = filterLen;
for filterIdx=1:dict1.numFilters
  for chanIdx = 1:dict1.numChans
    tempWave = randn(1,filterLen);
    dict1.filters(filterIdx).chans(chanIdx).wave = tempWave/sqrt(sum(tempWave*tempWave'));
  end
end

nyq = (floor(filterLen/2)*2 == filterLen);
nyqIdx = filterLen/2+1;
dict3.numFilters = numFilters;
dict3.numChans = numChansMono;
dict3.filterLen = filterLen;
for filterIdx=1:dict3.numFilters
  for chanIdx = 1:dict3.numChans
    tempWave = dict1.filters(filterIdx).chans(chanIdx).wave;
    fTempWave = fft(tempWave);
    fTempWave(1) = 0;
    if (nyq)
      fTempWave(nyqIdx) = 0;
    end
    tempWave = ifft(fTempWave);
    dict3.filters(filterIdx).chans(chanIdx).wave = tempWave/sqrt(sum(tempWave*tempWave'));
  end
end

dict2.numFilters = numFilters;
dict2.numChans = numChansStereo;
dict2.filterLen = filterLen;
for filterIdx=1:dict2.numFilters
  tempSum = 0;
  for chanIdx = 1:dict2.numChans
    tempWave = randn(1,filterLen);
    tempSum = tempSum + tempWave*tempWave';
    dict2.filters(filterIdx).chans(chanIdx).wave = tempWave;
  end
  tempSum = 1/sqrt(tempSum);
  for chanIdx = 1:dict2.numChans
    dict2.filters(filterIdx).chans(chanIdx).wave = dict2.filters(filterIdx).chans(chanIdx).wave * tempSum;
  end
  
end



% create the signals


% signal 1

sig1 = zeros(1,sigLen);

sig1FiltIdx = randint(1,numUsedFilters,[1 dict1.numFilters]);
sig1FiltLoc = randint(1,numUsedFilters,[1 (sigLen-dict1.filterLen+1)]);
sig1FiltAmpl = randn(1,numUsedFilters);
for filterIdx = 1:numUsedFilters
  sig1(sig1FiltLoc(filterIdx)+(0:(dict1.filterLen-1))) = dict1.filters(sig1FiltIdx(filterIdx)).chans(1).wave * sig1FiltAmpl(filterIdx);
end
sig1Factor = 0.95/max(abs(sig1(:)));
sig1 = sig1 * sig1Factor;

% signal 2

sig2 = zeros(2,sigLen);

sig2FiltIdx = randint(1,numUsedFilters,[1 dict1.numFilters]);
sig2FiltChanIdx = randint(1,numUsedFilters,[1 2]);
sig2FiltLoc = randint(1,numUsedFilters,[1 (sigLen-dict1.filterLen+1)]);
sig2FiltAmpl = randn(1,numUsedFilters);
for filterIdx = 1:numUsedFilters
  sig2(sig2FiltChanIdx(filterIdx),sig2FiltLoc(filterIdx)+(0:dict1.filterLen-1)) = dict1.filters(sig2FiltIdx(filterIdx)).chans(1).wave * sig2FiltAmpl(filterIdx);
end
sig2Factor = 0.95/max(abs(sig2(:)));
sig2 = sig2 * sig2Factor;


% signal 3

sig3 = zeros(2,sigLen);

sig3FiltIdx = randint(1,numUsedFilters,[1 dict1.numFilters]);
sig3FiltLoc = randint(1,numUsedFilters,[1 (sigLen-dict1.filterLen+1)]);
sig3FiltAmpl = randn(2,numUsedFilters);
for filterIdx = 1:numUsedFilters
  for chanIdx = 1:2
    sig3(chanIdx,sig3FiltLoc(filterIdx)+(0:dict1.filterLen-1)) = dict1.filters(sig3FiltIdx(filterIdx)).chans(1).wave * sig3FiltAmpl(chanIdx,filterIdx);
  end
end
sig3Factor = 0.95/max(abs(sig3(:)));
sig3 = sig3 * sig3Factor;


% signal 4

sig4 = zeros(2,sigLen);

sig4FiltIdx = randint(1,numUsedFilters,[1 dict2.numFilters]);
sig4FiltLoc = randint(1,numUsedFilters,[1 (sigLen-dict2.filterLen+1)]);
sig4FiltAmpl = randn(1,numUsedFilters);
for filterIdx = 1:numUsedFilters
  for chanIdx = 1:2
    sig4(chanIdx,sig4FiltLoc(filterIdx)+(0:dict2.filterLen-1)) = dict2.filters(sig4FiltIdx(filterIdx)).chans(chanIdx).wave * sig4FiltAmpl(1,filterIdx);
  end
end
sig4Factor = 0.95/max(abs(sig4(:)));
sig4 = sig4 * sig4Factor;

% signal 5

sig5 = randn(1,sigLen);
sig5Factor = 0.95/max(abs(sig5(:)));
sig5 = sig5 * sig5Factor;

% signal 6

sig6 = randn(1,sigLen);
sig6Factor = 0.95/max(abs(sig6(:)));
sig6 = sig6 * sig6Factor;

% saves

samplingFreq = 16000;
bitNum = 16;

% workspace
save([savePath '/ANYWAVE_synthetic_data.mat']);

% signals
wavwrite(sig1',samplingFreq, bitNum, [savePath '/sig1.wav']);
wavwrite(sig2',samplingFreq, bitNum, [savePath '/sig2.wav']);
wavwrite(sig3',samplingFreq, bitNum, [savePath '/sig3.wav']);
wavwrite(sig4',samplingFreq, bitNum, [savePath '/sig4.wav']);
wavwrite(sig5',samplingFreq, bitNum, [savePath '/sig5.wav']);
wavwrite(sig6',samplingFreq, bitNum, [savePath '/sig6.wav']);

% tables
savetable( dict1, [savePath '/table1.xml'], [savePath '/table1_data.bin'] );
savetable( dict2, [savePath '/table2.xml'], [savePath '/table2_data.bin'] )
savetable( dict3, [savePath '/table3.xml'], [savePath '/table3_data.bin'] );

% dictionaries
fid = fopen( [savePath '/dict1.xml'], 'wt');

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<dict>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');
fprintf(fid, '\t<block>\n');
fprintf(fid, '\t<param name="type" value="anywave"/>\n');
fprintf(fid, '\t\t<param name="tableFileName" value="%s"/>\n', [savePath '/table1.xml']);
fprintf(fid, '\t\t<param name="windowShift" value="%i"/>\n', filterShift);
fprintf(fid, '\t</block>\n');
fprintf(fid, '</dict>');

fclose( fid );

fid = fopen( [savePath '/dict2.xml'], 'wt');

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<dict>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

fprintf(fid, '\t<param name="type" value="anywave"/> \n');
fprintf(fid, '\t\t<param name="tableFileName" value="%s"/>\n', [savePath '/table2.xml']);
fprintf(fid, '\t\t<param name="windowShift" value="%i"/>\n', filterShift);
fprintf(fid, '\t</block>\n');
fprintf(fid, '</dict>');

fclose( fid );

fid = fopen( [savePath '/dict3.xml'], 'wt');

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<dict>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

fprintf(fid, '\t<param name="type" value="anywave"/> \n');
fprintf(fid, '\t\t<param name="tableFileName" value="%s"/>\n', [savePath '/table3.xml']);
fprintf(fid, '\t\t<param name="windowShift" value="%i"/>\n', filterShift);
fprintf(fid, '\t</block>\n');
fprintf(fid, '</dict>');

fclose( fid );

fid = fopen( [savePath '/dict3_hilbert.xml'], 'wt');

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<dict>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

fprintf(fid, '\t<param name="type" value="anywavehilbert"/>\n');
fprintf(fid, '\t\t<param name="tableFileName" value="%s"/>\n', [savePath '/table3.xml']);
fprintf(fid, '\t\t<param name="windowShift" value="%i"/>\n', filterShift);
fprintf(fid, '\t</block>\n');
fprintf(fid, '</dict>');

fclose( fid );
