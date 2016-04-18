# find_face_landmarks
A Matlab MEX function for finding face landmarks and bounding box using dlib.

This MEX is released as part of the face frontalization project:
http://www.openu.ac.il/home/hassner/projects/frontalize

If you find this code useful, please make sure to add suitable references to the original DLIB library and the frontalization paper. Bib items for both are:

DLIB
@article{king2009dlib,
  title={Dlib-ml: A machine learning toolkit},
  author={King, Davis E},
  journal=JMLR,
  volume={10},
  pages={1755--1758},
  year={2009},
  publisher={JMLR. org}
}

Frontalization
@inproceedings{HHPE:CVPR15:frontalize,
 author    = {Tal Hassner and Shai Harel and Eran Paz and Roee Enbar},
 title     = {Effective Face Frontalization in Unconstrained Images},
 booktitle = {IEEE Conf. on Computer Vision and Pattern Recognition (CVPR)},
 month	=  {June},
 year 	= {2015},
 URL 	= {\url{http://www.openu.ac.il/home/hassner/projects/frontalize}}
}

## Dependencies
1. [vsal](https://github.com/YuvalNirkin/vsal)
2. [dlib](http://dlib.net/)
3. [OpenCV](http://opencv.org/)
4. [Matlab](http://www.mathworks.com/products/matlab/)

## Installation
- Use CMake to build and install the mex function.
- Run find_face_landmarks_setup.m from the install directory.
- Download the landmarks model file: [shape_predictor_68_face_landmarks.dat](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2)
- Use show_face_landmarks function to display the output from find_face_landmarks

## Credit
Created by Yuval Nirkin.

https://il.linkedin.com/in/ynirkin
