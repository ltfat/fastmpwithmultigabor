function savetable( table, tableFileName, dataFileName )

% SAVETABLE Save a wavetable file from a table in Matlab
%
%    savetable( table, 'tableFileName', 'dataFileName',
%                    endianType ) 
%    saves the waveforms from the struct table to the
%    wavetable file 'tableFileName', while the binary data file is
%    'dataFileName', in LITTLE ENDIAN
%
%    table struct has fields :
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
%%   $Date: 2007-07-13 16:20:18 +0200 (Fri, 13 Jul 2007) $
%%   $Revision: 1108 $
%%


% fill the table file

fid = fopen( tableFileName, 'w');
if (fid == -1),
   error( [ 'Can''t open file [' tableFileName ']' ] );
end;

fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<table>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');

fprintf(fid, '\t<param name="numChans" value="%i"/>\n', table.numChans);
fprintf(fid, '\t<param name="filterLen" value="%i"/>\n', table.filterLen);
fprintf(fid, '\t<param name="numFilters" value="%i"/>\n', table.numFilters);
fprintf(fid, '\t<param name="data" value="%s"/>\n', dataFileName);
fprintf(fid, '</table>');

fclose( fid );

% fill the data file

fid = fopen( dataFileName, 'wb', 'l');
if (fid == -1),
   error( [ 'Can''t open file [' dataFileName ']' ] );
end;

for filterIdx = 1:table.numFilters,
  for chanIdx = 1:table.numChans,
    fwrite(fid ,table.filters(filterIdx).chans(chanIdx).wave, 'double');
  end;
end;

fclose( fid );
