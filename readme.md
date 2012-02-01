ProCamToolkit is a collection software and code for [openFrameworks](http://openframeworks.cc/) aimed at making high level projector-camera calibration more accessible. It provides examples manual projector calibration using a model of a scene, projector-camera calibration using a reference pattern and gray code scanning, and multi-projector calibration using gray code scanning. A program called "mapamok" has emerged from ProCamToolkit as the most user-friendly app of the collection, and it's useful for experimenting with extremely fast projection mapping in situations where the scene can be modeled quickly (e.g., a collection of cuboids) or where the scene is modeled in advance (e.g., a 3D print, or a manufactured object like a car). Most of ProCamToolkit is written in an extremely modular way, making it possible to borrow snippets of code, including some chunks that are written with the goal of being contributed to the openFrameworks core.

ProCamToolkit also includes work in progress towards new installations being developed during Guest Research Projector v.1 at [YCAM Interlab](http://interlab.ycam.jp/en/). This includes experiments with augmented shadows using multiple projectors, and projection mapping in the YCAM library.

ProCamToolkit is available under the [MIT License](https://secure.wikimedia.org/wikipedia/en/wiki/Mit_license).

# Apps

## Geometry

### mapamok

<iframe src="http://player.vimeo.com/video/35939205?title=0&amp;byline=0&amp;portrait=0" width="500" height="281" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe>

This is a complex app that will allow you to load a COLLADA file called `model.dae`, and then specify some number of points between the model and the corresponding location in the projection. After enough points have been selected, it will solve for the projector location and set the OpenGL viewport to render with the same intrinsics as the projector. For more details about mapamok, see [the wiki](https://github.com/YCAMInterlab/ProCamToolkit/wiki).

### ProCamSampleEdsdk and ProCamSampleLibdc

This app is the first step in capturing the data to calibrate a projector-camera system. It will project and capture gray code patterns. If you are scanning a scene, you only need to do this once. If you are calibrating before scanning a scene, you will need to take one scan per detected pattern.

### CameraCalibrate

This app will calibrate a camera from a sequence of images stored on disk, and save the calibration information into a `.yml` file.

### ShadowCast

<iframe src="http://player.vimeo.com/video/35992437?title=0&amp;byline=0&amp;portrait=0" width="500" height="281" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe>

ShadowCast is work in progress on an installation that explores augmented shadows using multiple projectors.

### ProCamScan

<iframe src="http://player.vimeo.com/video/34272491?title=0&amp;byline=0&amp;portrait=0" width="500" height="281" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe>

Decodes scan data and projection maps onto the point cloud.

## Color

### CameraGammaSampleLibdc

This is app is the first step in capturing the data to gamma calibrate a camera.

### CameraGammaCalibrate

Once you've captured the data from the above app, you can feed it into this one to recover the gamma curve of the camera.

# Examples

### ModelSelectionExample

Demonstrates how screen-space model selection is implemented for the ModelProjector app.

### RecursiveAverageExample

Recursively averages all the images in a folder. Useful for extracting gray images from a sequence of gray code scans.

# Other

### SharedCode

Includes `ofxProCamToolkit`, which is the core code backing the decoding involved in the other apps. Also `PatternGenerator` and `GrayCodeGenerator` are responsible for creating the projected patterns used in the other apps. Finally, it includes `scenes/`, a variety of generative visuals driven by user interaction. Each of these is used as a test scene for projector alignment.

### SharedData

Camera calibration and gamma calibration information shared between apps is saved here. There is also a small collection of useful shaders.

### Scenes

Demonstrates all of the scenes inside `SharedCode/scenes/`.

- - --

*ProCamToolkit is codeveloped by [YCAM Interlab](http://interlab.ycam.jp/en/).*