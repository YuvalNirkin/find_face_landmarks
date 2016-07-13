function show_face_landmarks(inputPath, frames)
%SHOW_FACE_LANDMARKS Show face landmarks
%   Input:
%       inputPath - Path to a directory containing a sequence of images or
%       a video file
%       frames - An array of frames, each frame contains the width and
%       height of the frame and an array of faces that were detected in
%       that frame. Each face contain its bounding box in the format 
%       [x y width height], and its detected landmarks as a n-by-2 matrix
%       (n is the number of points in the model). 

    if(isdir(inputPath))
        show_face_landmarks_dir(inputPath, frames);
    else
        show_face_landmarks_vid(inputPath, frames)
    end
end

function show_face_landmarks_dir(inputPath, frames)
    %% Parse directory
    filt = '.*(png|jpg)';
    fileDescs = dir(inputPath);
    fileNames = {fileDescs(~cellfun(@isempty,regexpi({fileDescs.name},filt))).name};

    if(length(fileNames) ~= length(frames))
        error('The number of images must be equal to the number of frames');
    end

    %% For each image
    for i = 1:min(length(fileNames), length(frames))
        imshow(fullfile(inputPath, fileNames{i}));
        hold on
        show_landmarks_frame(frames(i));
        drawnow
    end
end

function show_face_landmarks_vid(inputPath, frames)
    videoReader = VideoReader(inputPath);
    for i = 1:min(videoReader.NumberOfFrames, length(frames))
        frame = read(videoReader, i);
        imshow(frame);
        hold on;
        show_landmarks_frame(frames(i));
        drawnow
    end
end

function show_landmarks_frame(frame)
    for f = 1:length(frame.faces)
        rectangle('Position', frame.faces(f).bbox, 'EdgeColor', 'r');
        x = frame.faces(f).landmarks(:,1);
        y = frame.faces(f).landmarks(:,2);
        scatter(x, y, 'g.');
    end
end