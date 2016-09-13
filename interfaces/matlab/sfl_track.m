function sfl_track(varargin)
%SFL_TRACK Do tracking only on specified landmarks sequence.
%   SFL_TRACK(inputPath):
%   inputPath - Path to landmarks sequence or video sequence.
%   If it's a path to a video sequence then the landmarks sequence will be
%   searched for in the current directory. If it's a path to a landmarks
%   sequence then the video sequence will be taken from the link in the
%   file.
%
%SFL_TRACK(videoPath, landmarksPath)
%SFL_TRACK(landmarksPath, videoPath):
%   videoPath - Path to video sequence.
%   landmarksPath - Path to landmarks sequence (.lms).
%
%   Optional input:
%   'output' - Path to output file or directory.
%   'track' (=1) - Tracker type [1=BRISK|2=LBP].
%   'preview (=1) - Enable or disable preview of landmarks.

%% Parse input arguments
p = inputParser;
addRequired(p, 'videoPath', @ischar);
if(mod(nargin, 2) == 0)
    addRequired(p, 'landmarksPath', @ischar);
end
addParameter(p, 'output', '', @ischar);
addParameter(p, 'track', 1, @isscalar);
addParameter(p, 'preview', 1, @isscalar);
parse(p,varargin{:});

%% Execute sequence face landmarks track
exeName = 'sfl_track';
cmd = [exeName ' -o "' p.Results.output '"'...
    ' -t ' num2str(p.Results.track)...
    ' -p ' num2str(p.Results.preview)...
    ' "' p.Results.videoPath '"'];
if(mod(nargin, 2) == 0)
    cmd = [cmd  ' "' p.Results.landmarksPath '"'];
end
[status, cmdout] = system(cmd);
if(status ~= 0)
    error(cmdout);
end

