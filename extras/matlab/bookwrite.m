function bookwrite( book , fileName )

% BOOKREAD Exports a binary Matching Pursuit book from Matlab
%
%    bookwrite( book , fileName ) writes the binary format book
%    file 'fileName' from its as a structure.
%
%    See also BOOKREAD, BOOKPLOT, BOOKOVER.

%%
%% Author:
%% Thomas BlumensathQueen Mary, University of London                                              
%%
%% Distributed under the General Public License.
%%
%% Under development
%% The writing of Harmonic atoms is not tested yet.
%% Error handling not implemented yet.
%% Routine to determine if book structure is correct not implemented yet.
%%
                        
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
    fwrite( fid, book.numChans,'int');
    for ( c = 1:book.numChans ),
    fwrite( fid, book.atom{i}.pos(c),'ulong');   
    fwrite( fid, book.atom{i}.len(c),'ulong');  
    end;
 



     switch book.atom{i}.type,

	   case 'gabor',
 	   fprintf( fid, '%s\n', book.atom{i}.windowType);
 	   fwrite( fid, book.atom{i}.windowOpt, 'double' );
 	   fwrite( fid, book.atom{i}.freq, 'double' );
 	   fwrite( fid, book.atom{i}.chirp, 'double' );
 	   fwrite( fid, book.atom{i}.amp, 'double' );
 	   fwrite( fid, book.atom{i}.phase, 'double' );
 
 	   case 'harmonic',
       fprintf( fid, '%s\n', book.atom{i}.windowType);
 	   fwrite( fid, book.atom{i}.windowOpt, 'double' );
 	   fwrite( fid, book.atom{i}.freq, 'double' );
 	   fwrite( fid, book.atom{i}.chirp, 'double' );
 	   fwrite( fid, book.atom{i}.amp, 'double' );
 	   fwrite( fid, book.atom{i}.phase, 'double' );
 	   fwrite( fid, book.atom{i}.numPartials, 'unsigned int' );
       fwrite( fid, book.atom{i}.harmonicity , 'double' );
       fwrite( fid, book.atom{i}.partialAmpStorage, 'double' );
       fwrite( fid, book.atom{i}.partialPhaseStorage, 'double' );

 	   case 'dirac',
       fwrite( fid, book.atom{i}.amp, 'double' );
    
     end
end

% Write the closing tag
fprintf( fid ,'</book>\n');


fclose(fid);