% Add current folder and all subfolders to search path
scriptPath = mfilename('fullpath');
[rootPath, filename, fileextension]= fileparts(scriptPath);
addpath(fullfile(rootPath, 'mex'));
addpath(fullfile(rootPath, 'utilities'));