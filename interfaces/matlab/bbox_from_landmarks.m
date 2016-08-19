function bbox = bbox_from_landmarks(landmarks, frameWidth, frameHeight, square)
%BBOX_FROM_LANDMARKS(landmarks, frameWidth, frameHeight, square) Compute
%   bounding box from landmarks.
%   Input:
%   Output:
%       bbox - Output bounding box [minx miny width height].

%% Parse input arguments
if(~exist('square','var'))
    square = 1;
end

%% Calculate bounding box
minp = min(landmarks);
maxp = max(landmarks);
size = double(maxp - minp + 1);
center = double((maxp + minp)/2);
avg = round(mean(landmarks));
dev = center - avg;
dev_lt = round([0.1*size(1) size(2)*(max(size(1)/size(2),1)*2-1)]) +...
    abs(min(dev,0));
dev_rb = round(0.1*size) + max(dev,0);

%% Limit to frame boundaries
minp = max(double(minp) - dev_lt, 1);
maxp = min(double(maxp) + dev_rb, [frameWidth frameHeight]);

%% Make square
if(square)
    size = maxp - minp + 1;
    sq_size = max(size);
    half_sq_size = round((sq_size - 1)/2);
    center = round((maxp + minp)/2);
    minp = center - half_sq_size;
    maxp = center + half_sq_size;
    
    % Limit to frame boundaries
    minp = max(minp, 1);
    maxp = min(maxp, [frameWidth frameHeight]);
end

%% Output bounding box
bbox = [minp (maxp - minp + 1)];

end

