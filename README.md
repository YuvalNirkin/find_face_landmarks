# find_face_landmarks
Created by Yuval Nirkin.

[nirkin.com](http://www.nirkin.com/)

## Overview
This library provides video\image sequence functionality for finding face landmarks and bounding boxes.

![alt text](https://3.bp.blogspot.com/-bk69Sd5LTHk/V25XMfVMY1I/AAAAAAAAC9A/jiP6e5geTUQxAo8WCp36Z3L3CqlWqzbxgCLcB/s400/076_small_landmarks.PNG "Demonstration")

Main features:
- Matlab interface.
- Saving and loading per sequence.
- Face tracking across frames in a sequence.
- Face statistics for finding the most dominant face.

This library is released as part of the face frontalization project:
http://www.openu.ac.il/home/hassner/projects/frontalize

## Dependencies
| Library                                                            | Minimum Version | Notes                                    |
|--------------------------------------------------------------------|-----------------|------------------------------------------|
| [Boost](http://www.boost.org/)                                     | 1.47            |                                          |
| [OpenCV](http://opencv.org/)                                       | 3.0             |                                          |
| [dlib](http://dlib.net/)                                           | 18.18           |                                          |
| [vsal](https://github.com/YuvalNirkin/vsal)                        | 1.0             |                                          |
| [OpenCV's extra modules](https://github.com/opencv/opencv_contrib) | 3.0             | Optional - For the LBP face tracker      |
| [protobuf](https://github.com/google/protobuf)                     | 3.0.0           | Optional - For loading and saving        |
| [Matlab](http://www.mathworks.com/products/matlab/)                | 2012a           | Optional - For building the MEX function |

## Installation
- Use CMake to build and install the mex function.
- Add find_face_landmarks/bin to path.
- Add find_face_landmarks/interfaces/matlab to Matlab's path
- Download the landmarks model file: [shape_predictor_68_face_landmarks.dat](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2)
- Use show_face_landmarks function to display the output from find_face_landmarks

## Bibliography
If you find this code useful, please make sure to add suitable references to the original DLIB library and the frontalization paper. Bib items for both are:<br />
<br />
DLIB<br />
@article{king2009dlib,<br />
  title={Dlib-ml: A machine learning toolkit},<br />
  author={King, Davis E},<br />
  journal={J. Mach. Learning Research},<br />
  volume={10},<br />
  pages={1755--1758},<br />
  year={2009},<br />
  publisher={JMLR. org}<br />
}<br />
<br />
Frontalization<br />
@inproceedings{HHPE:CVPR15:frontalize,<br />
 author    = {Tal Hassner and Shai Harel and Eran Paz and Roee Enbar},<br />
 title     = {Effective Face Frontalization in Unconstrained Images},<br />
 booktitle = {IEEE Conf. on Computer Vision and Pattern Recognition (CVPR)},<br />
 month	=  {June},<br />
 year 	= {2015},<br />
 URL 	= {Available:~http://www.openu.ac.il/home/hassner/projects/frontalize}<br />
}<br />
