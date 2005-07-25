function [dict] = dictread( fileName )

%
% DICTREAD Imports a XML Matching Pursuit dictionary in Matlab
%
%    UNDER DEVELOPMENT, DO NOT USE.
%

%%
%% Authors:
%% Sacha Krstulovic & Rémi Gribonval
%% Copyright (C) 2005 IRISA                                              
%%
%% This script is part of the Matching Pursuit Library package,
%% distributed under the General Public License.
%%
%% SVN log:
%%   $Author$
%%   $Date$
%%   $Revision$
%%

fid = fopen( fileName );
if (fid == -1),
   error( [ 'Can''t open file [' fileName ']' ] );
end;

% Trash the XML def line
l = fgets(fid);

% Get the dict def line
l = fgets(fid);
[dict.numBlocks,c,e,nextIndex] = sscanf( l, '<dict numBlocks="%d"');
[dict.libVersion,c] = sscanf( l(nextIndex:end), ' libVersion="%[0-9a-z.]">' );
if ( c ~= 1 ),
   fclose(fid);
   error('Failed to scan the lib version.');
end;

% Get the blocks
for ( i = 1:dict.numBlocks );

    % Get the block type
    l = fgets( fid );
    if ( l == -1 ),
       fclose(fid);
       error( [ 'Can''t read a block type line for block number [' num2str(i) '].' ] );
    end;
    blockType = sscanf( l, ' <block type="%[a-z]">\n' );
    dict.block{i}.type = blockType;

    switch blockType,

	   case 'gabor',
	   l = fgets( fid ); dict.block{i}.filterLen = sscanf( l, ' <par type="filterLen">%lu</par>\n' );
	   l = fgets( fid ); dict.block{i}.filterShift = sscanf( l, ' <par type="filterShift">%lu</par>\n' );
	   l = fgets( fid ); dict.block{i}.numFilters = sscanf( l, ' <par type="numFilters">%lu</par>\n' );
	   l = fgets( fid );
	   [dict.block{i}.windowType,c,e,nextIndex] = sscanf( l, ' <window type="%[a-z]"');
	   dict.block{i}.windowOpt = sscanf( l(nextIndex:end), ' opt="%f"></window>' );

	   case 'harmonic',
	   l = fgets( fid ); dict.block{i}.filterLen = sscanf( l, ' <par type="filterLen">%lu</par>\n' );
	   l = fgets( fid ); dict.block{i}.filterShift = sscanf( l, ' <par type="filterShift">%lu</par>\n' );
	   l = fgets( fid ); dict.block{i}.numFilters = sscanf( l, ' <par type="numFilters">%lu</par>\n' );
	   l = fgets( fid );
	   [dict.block{i}.windowType,c,e,nextIndex] = sscanf( l, ' <window type="%[a-z]"');
	   dict.block{i}.windowOpt = sscanf( l(nextIndex:end), ' opt="%f"></window>' );
	   l = fgets( fid );
	   dict.block{i}.minFundFreqIdx = sscanf( l, ' <par type="minFundFreqIdx">%lu</par>\n' );
	   l = fgets( fid );
	   dict.block{i}.numFundFreqIdx = sscanf( l, ' <par type="numFundFreqIdx">%lu</par>\n' );
	   l = fgets( fid );
	   dict.block{i}.maxNumPartials = sscanf( l, ' <par type="maxNumPartials">%lu</par>\n' );

	   case 'dirac',
	   l = fgets( fid ); dict.block{i}.filterLen = 1;
	   l = fgets( fid ); dict.block{i}.filterShift = 1;
	   l = fgets( fid ); dict.block{i}.numFilters = 1;

	   % Unknown atom type
	   otherwise,
	   error( [ '[' blockType '] is an unknown atom type.'] );
    end;

    % Get the closing tag
    l = fgets( fid );
    if strcmp(l,'</block>\n'),
       error( ['Failed to read the closing </block> tag for block number ' num2str(i) '.'] );
    end;

end;

% Get the closing tag
l = fgets( fid );
if strcmp(l,'</dict>\n'),
   warning( 'Failed to read the closing </dict> tag.' );
end;


fclose(fid);
