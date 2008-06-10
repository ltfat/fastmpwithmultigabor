%MPTKinit;
fid = fopen(fullfile('~','.mat2mptk','MAT2MPTKconfig.txt'));
params = textscan(fid, '%s = %s');
fclose(fid);

%MPTKsettings = struct([]);

Variables = params{1};
Values = params{2};
for k = 1:size(params{1},1)
    eval(strcat('DefPropStruc(1,1).', Variables{k}, '=', Values{k},';'));
end

