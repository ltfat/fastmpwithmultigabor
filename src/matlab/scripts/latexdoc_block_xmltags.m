% Script to automatically generate latex code documenting the various XML
% tags used to describe blocks in dictionary description files


addpath /usr/local/mptk/matlab/
setenv('MPTK_CONFIG_FILENAME','/usr/local/mptk/path.xml')

info    = getmptkinfo;
blocks  = info.blocks;
nblocks = length(blocks);
strglob = '\begin{itemize}';

for n=1:nblocks
   block = blocks(n);
   np = length(block.parameters);
   strloc = ['\item[$\bullet$] Blocks of type: \texttt{' block.type '}\\'];
   % Find the index of the generic 'name' and'blockOffset' parameter
   if exist('pbo','var')
       clear pbo
   end
   for p=1:np
       if strcmp(block.parameters(p).name,'type');
           ptype = p;
       end
       if strcmp(block.parameters(p).name,'blockOffset');
           pbo   = p;
       end
   end
   % Order parameters by type / blockOffset / other
   if exist('pbo','var')
       paramlist = find((1:np)~=ptype & (1:np)~=pbo);
       paramlist = [ptype pbo paramlist];
   else
       paramlist = find((1:np)~=ptype);
       paramlist = [ptype paramlist];
   end
 
   % Provide general description of block
   parameter = block.parameters(ptype);
   strloc = [strloc '{\em ' parameter.info '}\\'];

   
   strloc = [strloc '\begin{itemize}'];
   for p=paramlist
      parameter = block.parameters(p);
      if p==ptype
          strloc = [strloc '\item[] ' '\texttt{<param name="' parameter.name '" value="' block.type '">}\\']; 
      else
          strloc = [strloc '\item[] ' '\texttt{<param name="' parameter.name '" value="VALUE">}\\']; 
      strloc = [strloc 'Type of the parameter:\ \texttt{' parameter.type '}\\'];
      strloc = [strloc 'Default value:\ \texttt{' parameter.default '}\\'];
      strloc = [strloc 'Description:\ {\em ' parameter.info '}\\'];       
            end

   end
   strloc= [strloc '\end{itemize}'];
   strglob = [strglob strloc];
end
strglob = [strglob '\end{itemize}'];

strglob = strrep(strglob,'_','\_');
strglob = strrep(strglob,'<','$<$');
strglob = strrep(strglob,'>','$>$');

disp(strglob)