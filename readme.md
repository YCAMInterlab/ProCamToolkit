ProCamToolkit is a collection software and code for [openFrameworks](http://openframeworks.cc/) aimed at making high level projector-camera calibration more accessible.

The ProCamToolkit also includes work in progress towards new installations being developed during my residency.

# Apps

## Geometry

### ProCamSampleEdsdk and ProCamSampleLibdc

This app is the first step in capturing the data to calibrate a projector-camera system. It will project and capture gray code patterns. If you are scanning a scene, you only need to do this once. If you are calibrating before scanning a scene, you will need to take one scan per detected pattern.

### CameraCalibrate

This app will calibrate a camera from a sequence of images stored on disk, and save the calibration information into a `.yml` file.

### ModelProjector

This is a fairly complex app that will allow you to load a Collada file `model.dae`, and then specify some number of points between the model and the corresponding location in the projection. After enough points have been selected, it will solve for the projector location and set the OpenGL viewport to render with the same intrinsics as the projector.

## Color

### CameraGammaSampleLibdc

This is app is the first step in capturing the data to gamma calibrate a camera.

### CameraGammaCalibrate

Once you've captured the data from the above app, you can feed it into this one to recover the gamma curve of the camera.

# Examples

### ModelSelectionExample

Demonstrates how screen-space model selection is implemented for the ModelProjector app.

### RecursiveAverageExample

Recursively averages all the images in a folder. Useful fro extracting gray images from a sequence of gray code scans.

# Other

### SharedCode

Includes `ofxProCamToolkit`, which is the core code backing the decoding involved in the other apps. Also `PatternGenerator` and `GrayCodeGenerator` are responsible for creating the projected patterns used in the other apps. Finally, it includes `scenes/`, a variety of generative visuals driven by user interaction. Each of these is used as a test scene for projector alignment.

### SharedData

Camera calibration and gamma calibration information shared between apps is saved here. There is also a small collection of useful shaders.

### Scenes

Demonstrates all of the scenes inside `SharedCode/scenes/`.

- - --

*ProCamToolkit is codeveloped by [YCAM Interlab](http://interlab.ycam.jp/en/).*