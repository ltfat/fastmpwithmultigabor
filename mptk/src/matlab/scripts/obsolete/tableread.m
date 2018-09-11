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
%% Sylvain Lesage & Sacha Krstulovic & R?mi Gribonval
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

fid = fopen( tableFileName, 'rt');
if (fid == -1),
   error( [ 'Can''t open file [' tableFileName ']' ] );
end;
fgets(fid);
fgets(fid);
fgets(fid);

a = fgets(fid);
table.numChans = sscanf(a, '\t<param name="numChans" value="%i"/>');
a = fgets(fid);
table.filterLen = sscanf(a, '\t<param name="filterLen" value="%i"/>');
a = fgets(fid);
table.numFilters = sscanf(a, '\t<param name="numFilters" value="%i"/>');
a = fgets(fid);
dataFileName = sscanf(a, '\t\t<param name="data" value="%s">');
dataFileName( strfind(dataFileName,'"/>') ) = [];
dataFileName( strfind(dataFileName,'/>') ) = [];
dataFileName( strfind(dataFileName,'>') ) = [];
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
