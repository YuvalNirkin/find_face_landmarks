function show_face_landmarks(inputPath, frames)
%SHOW_FACE_LANDMARKS Show face landmarks
%   Input:
%       inputPath - Path to a directory containing a sequence of images 
%       frames - An array of frames, each frame contains the width and
%       height of the frame and an array of faces that were detected in
%       that frame. Each face contain its bounding box in the format 
%       [x y width height], and its detected landmarks as a n-by-2 matrix
%       (n is the number of points in the model). 

%% Parse directory
filt = '.*(png|jpg)';
fileDescs = dir(inputPath);
fileNames = {fileDescs(~cellfun(@isempty,regexpi({fileDescs.name},filt))).name};

if(length(fileNames) ~= length(frames))
    error('The number of images must be equal to the number of frames');
end

%% For each image
for i = 1:length(fileNames)
    imshow(fullfile(inputPath, fileNames{i}));
    hold on
    for f = 1:length(frames(i).faces)
        rectangle('Position', frames(i).faces(f).bbox, 'EdgeColor', 'r');
        x = frames(i).faces(f).landmarks(:,1);
        y = frames(i).faces(f).landmarks(:,2);
        scatter(x, y, 'g.');
    end
    drawnow
end

