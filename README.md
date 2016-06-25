# find_face_landmarks
A Matlab MEX function for finding face landmarks and bounding box using dlib.

![alt text](https://3.bp.blogspot.com/-bk69Sd5LTHk/V25XMfVMY1I/AAAAAAAAC9A/jiP6e5geTUQxAo8WCp36Z3L3CqlWqzbxgCLcB/s400/076_small_landmarks.PNG "Demonstration")

This MEX is released as part of the face frontalization project:
http://www.openu.ac.il/home/hassner/projects/frontalize

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

## Dependencies
1. [vsal](https://github.com/YuvalNirkin/vsal)
2. [dlib](http://dlib.net/)
3. [OpenCV (3.0+)](http://opencv.org/)
4. [Matlab](http://www.mathworks.com/products/matlab/)

## Installation
- Use CMake to build and install the mex function.
- Run find_face_landmarks_setup.m from the install directory.
- Download the landmarks model file: [shape_predictor_68_face_landmarks.dat](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2)
- Use show_face_landmarks function to display the output from find_face_landmarks

## Credit
Created by Yuval Nirkin.

https://il.linkedin.com/in/ynirkin
