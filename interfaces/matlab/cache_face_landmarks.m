function cache_face_landmarks(varargin)
%CACHE_FACE_LANDMARKS Cache face landmarks for multiple executions
%   CACHE_FACE_LANDMARKS(input, landmarks, 'output', output,
%   'scale', scale, 'verbose', verbose):
%   input - Path to an image, a video file, a directory containing a
%       sequence of images, or a posix regular expression
%   output - Output file path or path to an output directory
%   landmarks - Path to landmarks model file
%   scale [=1] - Each frame will be scaled by this factor. Useful for
%       detection of small faces. The landmarks will still be in the
%       original frame's pixel coordinates
%   preview [=1] - Show preview of calculated landmarks

%% Parse input arguments
p = inputParser;
addRequired(p, 'input', @ischar);
addRequired(p, 'landmarks', @ischar);
addParameter(p, 'output', '', @ischar);
addParameter(p, 'scale', 1, @isscalar);
addParameter(p, 'preview', 1, @isscalar);
parse(p,varargin{:});

%% Execute cache face landmarks
exeName = 'cache_face_landmarks';
[status, cmdout] = system([exeName ' "' p.Results.input...
    '" -o "' p.Results.output '" -l "' p.Results.landmarks...
    '" -s "' num2str(p.Results.scale)...
    '" -p ' num2str(p.Results.preview)]);
if(status ~= 0)
    error(cmdout);
end

end

