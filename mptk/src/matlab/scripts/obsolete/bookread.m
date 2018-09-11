function [book] = bookread( fileName )
% MPTK - Matlab interface
% Imports a binary Matching Pursuit book file to Matlab, using MPTK
%
% WARNING: Will be deprecated as soon as MEX implementation is stable
%
% Usage : book = bookread(filename)
%
% Input : 
% filename : the filename where to read the book
%
% Output:
% book     : a book structure with the following structure
%    TODO
%
% Known limitations : only the following atom types are supported: 
%    gabor, harmonic, mdct, mclt, dirac.
%
% See also : bookwrite, bookplot, bookover
%
% Authors:
% Sacha Krstulovic, Remi Gribonval (IRISA, Rennes, France)
% Copyright (C) 2005 IRISA                                              
%
% Distributed under the General Public License.
%                                       
% This script is part of the Matching Pursuit Library package,
%

% Warn user that this file is no longer maintained by the team.
% Use Mex-Files instead!
warning( 'This file is no longer maintained and will soon be deprecated: MEX-files implementations are under development and the preferred way to read/write books' );

fid = fopen( fileName, 'r', 'l' );
if (fid == -1),
  error( [ 'Can''t open file [' fileName ']' ] );
end;

% Get the format line
l = fgets(fid);
if strcmp(l,'bin\n'),
  error( 'Bad book file, or book is not in bin format.' );
end;

% Get the header
l = fgets( fid );
[a,c,e,nextIndex] = sscanf( l, '<book nAtom="%lu" numChans="%d" numSamples="%lu" sampleRate="%d" ');
if ( c < 4 ),
  fclose(fid);
  error('Failed to scan the header.');
end;
book.format     = '0.0';
book.numAtoms   = a(1);
book.numChans   = a(2);
book.numSamples = a(3);
book.sampleRate = a(4);
[book.libVersion,c] = sscanf( l(nextIndex:end), 'libVersion="%[0-9a-z.]">\n' );
if ( c ~= 1 ),
  fclose(fid);
  error('Failed to scan the lib version.');
end;

% Get the atoms
for ( i = 1:book.numAtoms );

  % Get the atom type
  l = fgets( fid );
  if ( l == -1 ),
    fclose(fid);
    error( [ 'Can''t read an atom type for atom number [' num2str(i) '].' ] );
  end;
  atomType = sscanf( l, '%[a-z]\n' );
  book.atom{i}.type = atomType;
  % Get the generic atom parameters
  numChans = fread( fid, 1, 'ushort' );
  for ( c = 1:numChans ),
    pos(c) = fread( fid, 1, 'ulong' );
    len(c) = fread( fid, 1, 'ulong' );
  end;
  book.atom{i}.pos = pos;
  book.atom{i}.len = len;
  book.atom{i}.amp   = fread( fid, numChans, 'double' );  

  switch atomType,

   case 'gabor',
    l = fgets( fid );
    book.atom{i}.windowType = sscanf( l, '%[a-z]\n' );
    book.atom{i}.windowOpt = fread( fid, 1, 'double' );
    book.atom{i}.freq  = fread( fid, 1, 'double' );
    book.atom{i}.chirp = fread( fid, 1, 'double' );
    book.atom{i}.phase = fread( fid, numChans, 'double' );

   case 'harmonic',
    l = fgets( fid );
    book.atom{i}.windowType = sscanf( l, '%[a-z]\n' );
    book.atom{i}.windowOpt = fread( fid, 1, 'double' );
    book.atom{i}.freq  = fread( fid, 1, 'double' );
    book.atom{i}.chirp = fread( fid, 1, 'double' );
    book.atom{i}.phase = fread( fid, numChans, 'double' );
    numPartials = fread( fid, 1, 'unsigned int' );
    book.atom{i}.numPartials = numPartials;
    book.atom{i}.harmonicity = fread( fid, numPartials, 'double' );
    book.atom{i}.partialAmpStorage = fread( fid, numChans*numPartials, 'double' );
    book.atom{i}.partialAmpStorage = reshape( book.atom{i}.partialAmpStorage, numPartials, numChans );
    book.atom{i}.partialPhaseStorage = fread( fid, numChans*numPartials, 'double' );
    book.atom{i}.partialPhaseStorage = reshape( book.atom{i}.partialPhaseStorage, numPartials, numChans );

   case {'dirac','constant','nyquist'}

   case 'anywave'
    numChar = fread( fid, 1, 'ulong' );

    book.atom{i}.tableFileName = fread( fid, numChar, '*char' )';
    book.atom{i}.tableFileName(end) = [];
    book.atom{i}.filterIdx = fread( fid, 1, 'ulong' );

   case 'anywavehilbert'
    numChar = fread( fid, 1, 'ulong' );
    
    book.atom{i}.tableFileName = fread( fid, numChar, '*char' )';
    book.atom{i}.tableFileName(end) = [];
    book.atom{i}.filterIdx = fread( fid, 1, 'ulong' );
    realPart = fread( fid, numChans, 'double' );
    hilbertPart = fread( fid, numChans, 'double' );
    book.atom{i}.phase = atan2(hilbertPart,realPart);

   case 'mdct',
    l = fgets( fid );
    book.atom{i}.windowType = sscanf( l, '%[a-z]\n' );
    book.atom{i}.windowOpt = fread( fid, 1, 'double' );
    book.atom{i}.freq  = fread( fid, 1, 'double' );
    
   case 'mdst',
    l = fgets( fid );
    book.atom{i}.windowType = sscanf( l, '%[a-z]\n' );
    book.atom{i}.windowOpt = fread( fid, 1, 'double' );
    book.atom{i}.freq  = fread( fid, 1, 'double' );
    
   case 'mclt',
    l = fgets( fid );
    book.atom{i}.windowType = sscanf( l, '%[a-z]\n' );
    book.atom{i}.windowOpt = fread( fid, 1, 'double' );
    book.atom{i}.freq  = fread( fid, 1, 'double' );
    book.atom{i}.phase = fread( fid, numChans, 'double' ); 
    
    % Unknown atom type
   otherwise,
    error( [ '[' atomType '] is an unknown atom type.'] );
  end;

end;

% Get the closing tag
l = fgets( fid );
if strcmp(l,'</book>\n'),
  warning( 'Failed to read the closing </book> tag.' );
end;

fclose(fid);
