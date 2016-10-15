function sfl_cache(varargin)
%SFL_CACHE Cache face landmarks for multiple executions
%   SFL_CACHE(input, landmarks, 'output', output,
%   'scales', scales, 'track', track, 'preview', preview):
%   input - Path to an image, a video file, a directory containing a
%       sequence of images, or a posix regular expression
%   output - Output file path or path to an output directory
%   landmarks - Path to landmarks model file
%   scales [=1] - Each frame will be scaled by this factor. Useful for
%       detection of small faces. The landmarks will still be in the
%       original frame's pixel coordinates
%   track [=1] - Tracker type [0=NONE|1=BRISK|2=LBP].
%   preview [=1] - Show preview of calculated landmarks

%% Parse input arguments
p = inputParser;
addRequired(p, 'input', @ischar);
addRequired(p, 'landmarks', @ischar);
addParameter(p, 'output', '', @ischar);
addParameter(p, 'scales', 1, @isvector);
addParameter(p, 'track', 1, @isscalar);
addParameter(p, 'preview', 1, @isscalar);
parse(p,varargin{:});

%% Create scales string
scales = [];
for i = 1:length(p.Results.scales)
    scales = [scales ' -s ' num2str(p.Results.scales(i))];
end

%% Execute cache face landmarks
[status, cmdout] = system([mfilename ' "' p.Results.input...
    '" -o "' p.Results.output '" -l "' p.Results.landmarks...
    '" -t ' num2str(p.Results.track)...
    ' -p ' num2str(p.Results.preview)...
    scales]);
if(status ~= 0)
    error(cmdout);
end

end