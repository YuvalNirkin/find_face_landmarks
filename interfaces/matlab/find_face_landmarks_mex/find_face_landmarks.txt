%FIND_FACE_LANDMARKS Find face landmarks in a video, image sequence or
%camera stream.
%   frames = FIND_FACE_LANDMARKS(modelFile, input, scale, track, preview) 
%   Input:
%       modelFile - Path to the landmarks model file
%       input - An image or a path to an image, a video file, a directory
%       containing a sequence of images, or a posix regular expression
%       scale [=1] - Each frame will be scaled by this factor. Useful for
%       detection of small faces. The landmarks will still be in the
%       original frame's pixel coordinates
%       track [=1] - Tracker type [0=NONE|1=BRISK|2=LBP].
%       preview [=1] - Show preview of calculated landmarks
%
%   Output:
%       frames - An array of frames, each frame contains the width and
%       height of the frame and an array of faces that were detected in
%       that frame. Each face contain its bounding box in the format 
%       [x y width height], and its detected landmarks as a n-by-2 matrix
%       (n is the number of points in the model). 
%
%   frames = FIND_FACE_LANDMARKS(modelFile, device, width, height, scale, track)
%   this is the live version. device is the camera's id to start the 
%   preview from. width and height are the requested preview resolution.
%
%	frames = FIND_FACE_LANDMARKS(input) If input is a .lms file it will be
%   loaded, or a cache file by the name <video_name>_landmarks.lms will be
%   searched for in the same directory. If input is a landmarks model file,
%   it will be loaded and initialized to save time for future calls.
%
%   Examples
%       modelFile = 'shape_predictor_68_face_landmarks.dat';
%
%       % matlab image
%       I = imread('dataset_dir/img_01.jpg')
%       frames = find_face_landmarks(modelFile, I);
%
%       % single image
%       frames = find_face_landmarks(modelFile, 'dataset_dir/img_01.jpg');      
%
%       % multiple images (img_00.jpg, img_01.jpg, img_02.jpg, ...)
%       frames = find_face_landmarks(modelFile, 'dataset_dir/img_%02d.jpg');
%
%       % video file
%       frames = find_face_landmarks(modelFile, 'video.mp4');
%
%       % live camera stream
%       frames = find_face_landmarks(modelFile, 0);
%
%       % Load from cache
%       frames = find_face_landmarks('video.lms');
%
%       % Load from cache by searching for 'video.lms'
%       frames = find_face_landmarks('video.mp4');
%
%       % Initialize landmarks model file to save time for future calls
%       find_face_landmarks(modelFile);