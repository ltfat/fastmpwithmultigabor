function table = tableread( tableFileName )

% TABLEREAD read the wavetable from a tableFileName in Matlab
%
%    table = tableread( 'tableFileName' ) 
%    reads the waveforms from the wavetable file 'tableFileName'
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
%%   $Date$
%%   $Revision$
%%


% fill the table file

fid = fopen( tableFileName, 'rt');
if (fid == -1),
   error( [ 'Can''t open file [' tableFileName ']' ] );
end;
fgets(fid);
fgets(fid);
fgets(fid);

a = fgets(fid);
table.numChans = sscanf(a, '\t<par type = "numChans">%i</par>\n');
a = fgets(fid);
table.filterLen = sscanf(a, '\t<par type = "filterLen">%i</par>');
a = fgets(fid);
table.numFilters = sscanf(a, '\t<par type = "numFilters">%i</par>');
a = fgets(fid);
dataFileName = sscanf(a, '\t<par type = "data">%s');
dataFileName( strfind(dataFileName,'<') : end ) = [];

fclose( fid );

% fill the data file

fid = fopen( dataFileName, 'rb', 'l');
if (fid == -1),
   error( [ 'Can''t open file [' dataFileName ']' ] );
end;

for filterIdx = 1:table.numFilters,
  for chanIdx = 1:table.numChans,
    table.filters(filterIdx).chans(chanIdx).wave = fread(fid , table.filterLen, 'double');
  end;
end;

fclose( fid );
