function dictionary = createrandomdictionary( numFilters, numChans, ...
					      filterLen );

% CREATERANDOMDICTIONARY Create a random dictionary struct
%
%    dictionary = createrandomdictionary( numFilters, numChans,
%                 filterLen );
%
%    a dictionary struct has fields called :
%     - numFilters
%     - numChans
%     - filterLen
%     - filters : a struct array with the field :
%                - chans : a struct array with the field :
%                         - wave
%
%    each dictionary.atoms[i].chans[j] is a random vector of size
%    filterLen, and obviously there are numFilters atoms, and each
%    atom has numChans channels

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
%%   $Date: 2006-01-31 15:24:23 +0100 (Tue, 31 Jan 2006) $
%%   $Revision: 306 $
%%

dictionary.numFilters = numFilters;
dictionary.numChans = numChans;
dictionary.filterLen = filterLen;

for filterIdx = 1:numFilters,

  for chanIdx = 1:numChans,
    dictionary.filters(filterIdx).chans(chanIdx).wave = randn(1, filterLen);
  end;
end;
