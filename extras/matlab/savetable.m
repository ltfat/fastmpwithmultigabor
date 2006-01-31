function savetable( dictionary, tableFileName, dataFileName )

% SAVETABLE Save a wavetable file from a dictionary in Matlab
%
%    savetable( dictionary, 'tableFileName', 'dataFileName',
%                    endianType ) 
%    saves the waveforms from the struct dictionary to the
%    wavetable file 'tableFileName', while the binary data file is
%    'dataFileName', in LITTLE ENDIAN
%
%    dictionary struct has fields :
%     - numFilters
%     - numChans
%     - filterLen
%     - filters : a struct array with the field :
%                 - chans : a struct array with the field :
%                          - wave

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
%%   $Date$
%%   $Revision$
%%


% fill the table file

fid = fopen( tableFileName, 'wt');
if (fid == -1),
   error( [ 'Can''t open file [' tableFileName ']' ] );
end;

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

fprintf(fid, '<table>\n');
fprintf(fid, '\t<par type = "numChans">%i</par>\n', dictionary.numChans);
fprintf(fid, '\t<par type = "filterLen">%i</par>\n', dictionary.filterLen);
fprintf(fid, '\t<par type = "numFilters">%i</par>\n', dictionary.numFilters);
fprintf(fid, '\t<par type = "data">%s</par>\n', dataFileName);
fprintf(fid, '</table>');

fclose( fid );

% fill the data file

fid = fopen( dataFileName, 'wb', 'l');
if (fid == -1),
   error( [ 'Can''t open file [' dataFileName ']' ] );
end;

for filterIdx = 1:dictionary.numFilters,
  for chanIdx = 1:dictionary.numChans,
    fwrite(fid ,dictionary.filters(filterIdx).chans(chanIdx).wave, 'double');
  end;
end;

fclose( fid );
