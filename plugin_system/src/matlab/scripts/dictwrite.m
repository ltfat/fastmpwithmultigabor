function [dictFileName,tableFileNameList,dataTableFileNameList] = dictwrite( dict, fileName )

% [dictFileName,tableFileNameList,dataTableFileNameList] = dictwrite( dict, fileName )
%
% DICTWRITE Exports a XML Matching Pursuit dictionary from Matlab
%

%%
%% Authors:
%% Sylvain Lesage & Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% Warm user that this file is no longer maintained by the team.
%% Use Mex-Files instead!
warning( 'This file is no longer maintained' );

if (strcmp(fileName(end-3:end),'.xml') == 1)
  dictFileName = fileName;
  fileName(end-3:end) = [];
else
  dictFileName = [fileName '.xml'];
end

tableFileNameList = {};
dataTableFileNameList = {};

fidDict = fopen( dictFileName, 'wt');
if (fidDict == -1)
  fprintf('dictwrite.m - cannot open the dictionnary file [%s]. Exit',dictFileName);
  dictFileName = [];
  return;
end  

fprintf(fidDict, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fidDict, '<libVersion>0.4beta</libVersion>\n');
fprintf(fidDict, '<dict>\n');

numTables = 0;

dict.numBlocks = length(dict.block);

for blockIdx = 1:dict.numBlocks

  b = dict.block{blockIdx};
  switch (b.type)
   case 'dirac'
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    fprintf(fidDict, '\t</block>\n');    
   case {'constant','nyquist'}
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    fprintf(fidDict, '\t\t<par type = "windowShift">%lu</par>\n', b.filterShift );
    fprintf(fidDict, '\t\t<par type = "windowLen">%lu</par>\n', b.filterLen );
    fprintf(fidDict, '\t</block>\n');    
   case 'gabor'
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    fprintf(fidDict, '\t\t<par type = "windowShift">%lu</par>\n', b.filterShift );
    fprintf(fidDict, '\t\t<par type = "windowLen">%lu</par>\n', b.filterLen );
    fprintf(fidDict, '\t\t<par type = "fftSize">%lu</par>\n', b.numFilters );
    fprintf(fidDict, '\t\t<window type = "%s" opt = "%f"></window>\n', b.windowType, b.windowOpt );
    fprintf(fidDict, '\t</block>\n');    
   case 'harmonic'
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    fprintf(fidDict, '\t\t<par type = "windowShift">%lu</par>\n', b.filterShift );
    fprintf(fidDict, '\t\t<par type = "windowLen">%lu</par>\n', b.filterLen );
    fprintf(fidDict, '\t\t<par type = "fftSize">%lu</par>\n', b.numFilters );
    fprintf(fidDict, '\t\t<window type = "%s" opt = "%f"></window>\n', b.windowType, b.windowOpt );
    fprintf(fidDict, '\t\t<par type = "f0Min">%lu</par>\n', b.f0Min );
    fprintf(fidDict, '\t\t<par type = "f0Max">%lu</par>\n', b.f0Max );
    fprintf(fidDict, '\t\t<par type = "numPartials">%lu</par>\n', b.numPartials );
    fprintf(fidDict, '\t</block>\n');
   case 'chirp'
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    fprintf(fidDict, '\t\t<par type = "windowShift">%lu</par>\n', b.filterShift );
    fprintf(fidDict, '\t\t<par type = "windowLen">%lu</par>\n', b.filterLen );
    fprintf(fidDict, '\t\t<par type = "fftSize">%lu</par>\n', b.numFilters );
    fprintf(fidDict, '\t\t<window type = "%s" opt = "%f"></window>\n', b.windowType, b.windowOpt );
    fprintf(fidDict, '\t\t<par type = "numFitPoints">%u</par>\n', b.numFitPoints );
    fprintf(fidDict, '\t</block>\n');    
   case {'anywave','centeredanywave','anywavehilbert'}
    [tableFileName,dataTableFileName] = tablewrite( b, fileName, blockIdx );
    numTables = numTables + 1;
    tableFileNameList{numTables} = tableFileName;
    dataTableFileNameList{numTables} = dataTableFileName;
    
    if (strcmp(b.type,'centeredanywave') == 1)
      b.type = 'anywave';
    end
    fprintf(fidDict, '\t<block type="%s">\n',b.type);
    
    fprintf(fidDict, '\t\t<par type="tableFileName">%s</par>\n', tableFileName );
    fprintf(fidDict, '\t\t<par type="windowShift">%i</par>\n', b.filterShift );
    fprintf(fidDict, '\t</block>\n');    
  end
end

fprintf(fidDict, '</dict>');
fclose( fidDict );

return;

function [tableFileName,dataTableFileName] = tablewrite( block, fileName, tableIdx )

tableFileName = sprintf('%s_anywave_table_%0.3i.bin',fileName,tableIdx);
dataTableFileName = sprintf('%s_anywave_table_data_%0.3i.bin',fileName,tableIdx);

fid = fopen( tableFileName, 'wt');
fprintf(fid, '<?xml version="1.0" encoding="ISO-8859-1"?>\n');
fprintf(fid, '<libVersion>0.4beta</libVersion>\n');
fprintf(fid, '<table>\n');
fprintf(fid, '\t<par type = "numChans">%i</par>\n', block.numChans);
fprintf(fid, '\t<par type = "filterLen">%i</par>\n', block.filterLen);
fprintf(fid, '\t<par type = "numFilters">%i</par>\n', block.numFilters);
fprintf(fid, '\t<par type = "data">%s</par>\n', dataTableFileName);
fprintf(fid, '</table>');
fclose( fid );

fid = fopen( dataTableFileName, 'wb', 'l');
for k = 1:block.numFilters,
  for c = 1:block.numChans,
    fwrite(fid , block.data{k}(:,c) , 'double');
  end;
end;
fclose( fid );

return;
