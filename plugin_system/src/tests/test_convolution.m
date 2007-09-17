function test_convolution

% test_convolution
% 
% creates the data in Matlab for the test_convolution executable


% parameters
signalLen = 135;
numChansSignal = 1;
numChansFilters = 1;
numFilters = 4;
filterLen = 128;
filterShift = 1;

signalsPath = '/udd/slesage/MPTK/trunk/src/tests/signals/';
samplingFreq = 16000;
numBits = 32;


signalFileName = [signalsPath 'anywave_signal.wav'];
tableFileName = [signalsPath 'anywave_table.bin'];
dataFileName = [signalsPath 'anywave_table_data.bin'];

% creation of the signal
signal = randn(signalLen,numChansSignal);
signal = signal/max(max(abs(signal)))*0.95;

wavwrite(signal,samplingFreq,numBits,signalFileName);

% it is reread in order to use the same data as test_convolution.cpp (32 bit data for the signal)
signal = double(wavread(signalFileName));

fprintf('--wrote file %s\n',signalFileName);


% creation of the table
table.numFilters = numFilters;
table.numChans = numChansFilters;
table.filterLen = filterLen;
for n=1:numFilters,
  for m=1:numChansFilters,
    temp = randn(filterLen,1);
    temp = temp/sqrt(temp'*temp);
    table.filters(n).chans(m).wave = double(temp);
  end
end

savetable( table, tableFileName, dataFileName );
fprintf('--wrote file %s\n',tableFileName);
fprintf('--wrote file %s\n',dataFileName);

% computing results of the first experiment
% the inner product between each frame of signal and each filter, 
% saved in the following order : 
% Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results1 = zeros(numFrames * numFilters,1);
results1FileName = [signalsPath 'anywave_results_1.bin'];

for n=1:numFrames,
  for m=1:numFilters,
    results1( (m-1)*numFrames + n ) = signal( (n-1)*filterShift + (1:filterLen), 1 )' * table.filters(m).chans(1).wave;    
  end  
end

fid = fopen(results1FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results1,'double');
fclose(fid);
fprintf('--wrote file %s\n',results1FileName);

% computing results of the second experiment
% finding the max energy and the corresponding filter, for each frame of signal. 
% It is saved in the following order : 
% energies : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...
% indices : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results2Amp = zeros(numFrames,1);
results2Idx = zeros(numFrames,1);
results2FileName = [signalsPath 'anywave_results_2.bin'];

for n=1:numFrames,
  [results2Amp(n) results2Idx(n)] = max( results1( n + 0:numFrames:end ).^2 );
end

fid = fopen(results2FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results2Amp,'double');
% idx - 1 : in order to be coherent with C++ convention
fwrite(fid,results2Idx-1,'ulong');
fclose(fid);
fprintf('--wrote file %s\n',results2FileName);

% computing results of the third experiment
% finding the max energy and the corresponding filter, for each
% frame of signal, in the sens of Hilbert. 
% It is saved in the following order : 
% energies : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...
% indices : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results3 = zeros(numFrames*numFilters,1);
results3Amp = zeros(numFrames,1);
results3Idx = zeros(numFrames,1);
results3FileName = [signalsPath 'anywave_results_3.bin'];

% filter without mean and nyquist
% and
% filter transformed via Hilbert
for m=1:numFilters,
  f = fft(table.filters(m).chans(1).wave);
  f([1 filterLen/2+1]) = 0;
  tableM.filters(m).chans(1).wave = ifft(f);
  f(2:filterLen/2) = -sqrt(-1) * f(2:filterLen/2);
  f(filterLen/2+2:end) = sqrt(-1) * f(filterLen/2+2:end);
  tableH.filters(m).chans(1).wave = ifft(f);

  tableM.filters(m).chans(1).wave = tableM.filters(m).chans(1).wave/norm(tableM.filters(m).chans(1).wave);
  tableH.filters(m).chans(1).wave = tableH.filters(m).chans(1).wave/norm(tableH.filters(m).chans(1).wave);

end  
for n=1:numFrames,
  s = signal( (n-1)*filterShift + (1:filterLen), 1 )';
  for m=1:numFilters,
    c = s*tableM.filters(m).chans(1).wave*tableM.filters(m).chans(1).wave'*s';
    d = s*tableH.filters(m).chans(1).wave*tableH.filters(m).chans(1).wave'*s';
    results3( (m-1)*numFrames + n ) = c+d;    
  end  
end

for n=1:numFrames,
  [results3Amp(n) results3Idx(n)] = max( results3( n + 0:numFrames:end ) );
end

fid = fopen(results3FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results3Amp,'double');
% idx - 1 : in order to be coherent with C++ convention
fwrite(fid,results3Idx-1,'ulong');
fclose(fid);
fprintf('--wrote file %s\n',results3FileName);


matlabFileName = [signalsPath 'matlab_save.mat'];
save(matlabFileName);
fprintf('--wrote file %s\n',matlabFileName);
