% Script that installs Mat2MPTK
%version: 0.4.1 rev 1

% creates mat2mptk settings directory
if ~exist(fullfile('~','.mat2mptk'), 'file')
    mkdir(fullfile('~','.mat2mptk'));
end

if isunix
    filesep = '/';
else
    filesep = '\';
end

fid = fopen(fullfile('~','.mat2mptk','MAT2MPTKconfig.txt'), 'w');


MPTKPath = input('Enter MPTK Path [''@CMAKE_INSTALL_PREFIX@/bin/'']: ');
if isempty(MPTKPath), MPTKPath = '@CMAKE_INSTALL_PREFIX@/bin/';end

fprintf(fid, 'MPTKPath = ''%s''\n', MPTKPath);

DicoPath = input(['Enter Dictionaries Path [''' fullfile(pwd, 'Dict') filesep ''']: ']);

%DicoPath = input(['Enter Dictionaries Path [''' fullfile('~','.mat2mptk', 'Dict') filesep ''']: ']);

if isempty(DicoPath), DicoPath = [fullfile(pwd, 'Dict') filesep];end


%if isempty(DicoPath), DicoPath = [fullfile('~','.mat2mptk', 'Dict') filesep];end
if ~exist(DicoPath, 'file'), mkdir(DicoPath); end

fprintf(fid, 'DicoPath = ''%s''\n', DicoPath); 
fprintf(fid, 'D = ''default.xml''\n');
fprintf(fid, 'n = 10000\n');

fclose(fid);
%MPTKLoadSettings;
%GenerDict('default.xml');

clear fid DicoPath MPTKPath
