# Find Face Landmarks
![alt text](https://yuvalnirkin.github.io/assets/img/projects/find_face_landmarks/thumb.jpg "Demonstration")

Created by Yuval Nirkin.

[nirkin.com](http://www.nirkin.com/)

## Overview
This library provides video\image sequence functionality for finding face landmarks and bounding boxes using [dlib](http://dlib.net/).

Main features:
- Matlab interface.
- Saving and loading per sequence.
- Face tracking across frames in a sequence.
- Face statistics for finding the most dominant face.

Link for the demonstration video:

[![Demonstration Video](http://img.youtube.com/vi/mTW0zIrrkEI/0.jpg)](http://www.youtube.com/watch?v=mTW0zIrrkEI)

If you find this code useful, please make sure to cite our paper in your work:

Yuval Nirkin, Iacopo Masi, Anh Tuan Tran, Tal Hassner, Gerard Medioni, "[On Face Segmentation, Face Swapping, and Face Perception](https://arxiv.org/pdf/1704.06729.pdf)", arXiv preprint arXiv:1704.06729, 22 Apr 2017.

Please see [project page](http://www.openu.ac.il/home/hassner/projects/faceswap/) for more details, more resources and updates on this project.

## Usage
There are 3 ways to use the library:
- Matlab interface. Please take a look at the [MATLAB Tutorial](https://github.com/YuvalNirkin/find_face_landmarks/wiki/MATLAB-Tutorial).
- C++ interface. Please take a look at the [Doxygen generated documentation](https://yuvalnirkin.github.io/projects/find_face_landmarks/).
- Command line tools. Use --help for more information on each tool.

## Dependencies
| Library                                                            | Minimum Version | Notes                                    |
|--------------------------------------------------------------------|-----------------|------------------------------------------|
| [Boost](http://www.boost.org/)                                     | 1.47            |                                          |
| [OpenCV](http://opencv.org/)                                       | 3.0             |                                          |
| [dlib](https://github.com/davisking/dlib) or [dlib (Windows)](https://github.com/YuvalNirkin/dlib) | 18.18 |                    |
| [OpenCV's extra modules](https://github.com/opencv/opencv_contrib) | 3.0             | Optional - For the LBP face tracker      |
| [protobuf](https://github.com/google/protobuf)                     | 3.0.0           | Optional - For loading and saving        |
| [Matlab](http://www.mathworks.com/products/matlab/)                | 2012a           | Optional - For building the MEX function |

## Installation
- Use CMake and your favorite compiler to build and install the library or download the available binaries from [here](https://github.com/YuvalNirkin/find_face_landmarks/releases).
- Add find_face_landmarks/bin to path.
- Add find_face_landmarks/interfaces/matlab to Matlab's path
- Download the landmarks model file: [shape_predictor_68_face_landmarks.dat](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2)

## Bibliography
[1] Yuval Nirkin, Iacopo Masi, Anh Tuan Tran, Tal Hassner, Gerard Medioni, [On Face Segmentation, Face Swapping, and Face Perception](https://arxiv.org/pdf/1704.06729.pdf), arXiv preprint arXiv:1704.06729, 22 Apr 2017.  
[2] Davis E. King, [Dlib-ml: A Machine Learning Toolkit](http://www.jmlr.org/papers/volume10/king09a/king09a.pdf), Journal of Machine Learning Research, 2009.  
[3] V. Kazemi and J. Sullivan. [One millisecond face alignment with an ensemble of regression trees](http://www.cv-foundation.org/openaccess/content_cvpr_2014/papers/Kazemi_One_Millisecond_Face_2014_CVPR_paper.pdf). In Proc. Conf. Comput.Vision Pattern Recognition. IEEE, 2014
