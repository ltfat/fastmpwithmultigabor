function test_convolution(signalFileName, tableFileName, saveResultsPath)
%function test_convolution Interface for creating the data in Matlab for the test_convolution executable
%
%    test_convolution(signalFileName, tableFileName, dataFileName) generates
%
%

% parameters
filterShift = 1;

%------------------------------------------------------
% Reading the signal File (32 bit data for the signal)
%------------------------------------------------------
signal = double(wavread(signalFileName));

% Calculating the signal length
signalLen = size(signal,1);

%------------------------------------------------------
% Opening an anywave table
%------------------------------------------------------
table = anywavetableread(tableFileName);
% Getting the table parameters (filterLen = 1st size of wave, numchans = 2nd size of wave, numfilters = 3rd size of wave
filterLen = size(table.wave,1);
numChans = size(table.wave,2);
numFilters = size(table.wave,3);
						 
%------------------------------------------------------
% Computing results of the first experiment
%------------------------------------------------------
% The inner product between each frame of signal 
% and each filter, saved in the following order : 
% Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results1 = zeros(numFrames * numFilters,1);

for n=1:numFrames,
	for m=1:numFilters,
		results1( (m-1)*numFrames + n ) = signal( (n-1)*filterShift + (1:filterLen), 1)' * table.wave(:,:,m);
	end
end

% Write the datas
results1FileName = [saveResultsPath 'anywave_results_1.bin'];
fid = fopen(results1FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results1,'double');
fclose(fid);
fprintf('--wrote file %s\n',results1FileName);

%------------------------------------------------------
% Computing results of the second experiment
%------------------------------------------------------
% finding the max energy and the corresponding filter, for each frame of signal. 
% It is saved in the following order : 
% energies : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...
% indices : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results2Amp = zeros(numFrames,1);
results2Idx = zeros(numFrames,1);

for n=1:numFrames,
	[results2Amp(n) results2Idx(n)] = max( results1( n + 0:numFrames:end ).^2 );
end

% Write the datas (index - 1 : in order to be coherent with C++ convention)
results2FileName = [saveResultsPath 'anywave_results_2.bin'];
fid = fopen(results2FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results2Amp,'double');
fwrite(fid,results2Idx-1,'ulong');
fclose(fid);
fprintf('--wrote file %s\n',results2FileName);

%------------------------------------------------------
% Computing results of the third experiment
%------------------------------------------------------
% finding the max energy and the corresponding filter, for each
% frame of signal, in the sens of Hilbert. 
% It is saved in the following order : 
% energies : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...
% indices : Filter1-frame1,Filter1-frame2...Filter1-frameN,Filter2-frame1...

numFrames = floor((signalLen - filterLen)/filterShift)+1;
results3 = zeros(numFrames*numFilters,1);
results3Amp = zeros(numFrames,1);
results3Idx = zeros(numFrames,1);

% filter without mean and nyquist
% filter transformed via Hilbert
for m=1:numFilters,
  f = fft(table.wave(:,:,m));
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

% Write the datas (index - 1 : in order to be coherent with C++ convention)
results3FileName = [saveResultsPath 'anywave_results_3.bin'];
fid = fopen(results3FileName,'wb');
fwrite(fid,filterShift,'ulong');
fwrite(fid,results3Amp,'double');
fwrite(fid,results3Idx-1,'ulong');
fclose(fid);
fprintf('--wrote file %s\n',results3FileName);