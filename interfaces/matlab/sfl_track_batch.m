function sfl_track_batch(varargin)
%SFL_TRACK_BATCH Summary of this function goes here
%   Detailed explanation goes here

%% Parse input arguments
p = inputParser;
addRequired(p, 'inDir', @ischar);
addRequired(p, 'outDir', @ischar);
addParameter(p, 'track', 1, @isscalar);
addParameter(p, 'preview', 1, @isscalar);
addParameter(p, 'indices', [], @isvector);
parse(p,varargin{:});
indices = p.Results.indices;

%% Parse input directory
filt = '.*(lms|pb)';
fileDescs = dir(p.Results.inDir);
fileNames = {fileDescs.name};
fileNames = {fileDescs(~cellfun(@isempty,regexpi({fileDescs.name},filt))).name};
if(isempty(indices))
    indices = 1:length(fileNames);
elseif(max(indices) > length(fileNames) || min(indices) < 1)
    error(['indices must be from 1 to ' num2str(length(fileNames))]);
end

%% For each landmarks file
for i = indices
    lmsFile = fullfile(p.Results.inDir, fileNames{i});
    [~,lmsName,lmsExt] = fileparts(lmsFile);    
    disp(['Processing "', [lmsName lmsExt], '"']);
    
    %% track
    sfl_track(lmsFile, 'output', p.Results.outDir,...
        'track', p.Results.track, 'preview', p.Results.preview);
end
