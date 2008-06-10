function dec = ReadDecay(DecayFile)

fid = fopen(DecayFile, 'r','l');
dec = fread( fid, inf, 'double');
fclose(fid);
