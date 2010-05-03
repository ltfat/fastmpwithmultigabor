function bookwrite( book , fileName )
% MPTK - Matlab interface
% Exports a binary Matching Pursuit book from Matlab, as a script
%
% WARNING: Will be deprecated as soon as MEX implementation is stable
%
% Usage : bookwrite(book,filename ) 
%
% Input : 
% book     : a book structure with the following structure
%    TODO
% filename : the filename where to read the book
%
% Known limitations : only the following atom types are supported: 
%    gabor, harmonic, mdct, mclt, dirac.
%
% See also : bookread, bookplot, bookover
%
% Authors:
% Sacha Krstulovic, Remi Gribonval (IRISA, Rennes, France)
% Thomas Blumensath (Queen Mary, University of London)
% 
% Distributed under the General Public License.
%                                       
% The writing of Harmonic atoms is not tested yet.
% Error handling not implemented yet.
% Routine to determine if book structure is correct not implemented yet.
%

% Warn user that this file is no longer maintained by the team.
% Use Mex-Files instead!
warning( 'This file is no longer maintained and will soon be deprecated: MEX-files implementations are under development and the preferred way to read/write books' );

fid = fopen( fileName, 'w', 'l' );
if (fid == -1),
  error( [ 'Can''t open file [' fileName ']' ] );
end;

% Write the formating line
fprintf(fid,'bin\n');

% Write the header
fprintf(fid, '<book nAtom="%lu" numChans="%d" numSamples="%lu" sampleRate="%d" libVersion="%s">\n', book.numAtoms , book.numChans, book.numSamples , book.sampleRate , book.libVersion );

% Write the atoms
for ( i = 1:book.numAtoms );
  
  % Write the atom type
  fprintf(fid,'%s\n', book.atom{i}.type);
  
  % Write the generic atom parameters
  fwrite( fid, book.numChans,'ushort');
  for ( c = 1:book.numChans ),
    fwrite( fid, book.atom{i}.pos(c),'ulong');   
    fwrite( fid, book.atom{i}.len(c),'ulong');  
  end;
  fwrite( fid, book.atom{i}.amp,'double');

  switch book.atom{i}.type,

   case 'gabor',
    fprintf( fid, '%s\n', book.atom{i}.windowType);
    fwrite( fid, book.atom{i}.windowOpt, 'double' );
    fwrite( fid, book.atom{i}.freq, 'double' );
    fwrite( fid, book.atom{i}.chirp, 'double' );
    fwrite( fid, book.atom{i}.phase, 'double' );
    
   case 'harmonic',
    fprintf( fid, '%s\n', book.atom{i}.windowType);
    fwrite( fid, book.atom{i}.windowOpt, 'double' );
    fwrite( fid, book.atom{i}.freq, 'double' );
    fwrite( fid, book.atom{i}.chirp, 'double' );
    fwrite( fid, book.atom{i}.phase, 'double' );
    fwrite( fid, book.atom{i}.numPartials, 'unsigned int' );
    fwrite( fid, book.atom{i}.harmonicity , 'double' );
    fwrite( fid, book.atom{i}.partialAmpStorage, 'double' );
    fwrite( fid, book.atom{i}.partialPhaseStorage, 'double' );

   case 'anywave'
    fwrite( fid, length(book.atom{i}.tableFileName), 'ulong');
    fprintf( fid, '%s\n', book.atom{i}.tableFileName);
    fwrite( fid, book.atom{i}.waveIdx, 'ulong');

   case 'anywavehilbert'
    fwrite( fid, length(book.atom{i}.tableFileName), 'ulong');
    fprintf( fid, '%s\n', book.atom{i}.tableFileName);
    fwrite( fid, book.atom{i}.waveIdx, 'ulong');
    fwrite( fid, book.atom{i}.meanPart, 'double');
    fwrite( fid, book.atom{i}.nyquistPart, 'double');
    fwrite( fid, book.atom{i}.realPart, 'double');
    fwrite( fid, book.atom{i}.hilbertPart, 'double');
    
   case 'mdct',
    fprintf( fid, '%s\n', book.atom{i}.windowType);
    fwrite( fid, book.atom{i}.windowOpt, 'double' );
    fwrite( fid, book.atom{i}.freq, 'double' );
    
   case 'mdst',
    fprintf( fid, '%s\n', book.atom{i}.windowType);
    fwrite( fid, book.atom{i}.windowOpt, 'double' );
    fwrite( fid, book.atom{i}.freq, 'double' ); 
  
   case 'mclt',
    fprintf( fid, '%s\n', book.atom{i}.windowType);
    fwrite( fid, book.atom{i}.windowOpt, 'double' );
    fwrite( fid, book.atom{i}.freq, 'double' );
    fwrite( fid, book.atom{i}.phase, 'double' );

   otherwise
    error(['writing ' book.atom{i}.type ' atoms not implemented.');
  end
end

% Write the closing tag
fprintf( fid ,'</book>\n');


fclose(fid);
