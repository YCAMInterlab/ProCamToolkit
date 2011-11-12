#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	cameraMode = EASYCAM_MODE;

	FileStorage cfs(ofToDataPath("cam.yml"), FileStorage::READ);
	cfs["cameraMatrix"] >> camMatrix;
	camSize.width = cfs["imageSize_width"];
	camSize.height = cfs["imageSize_height"];
	cfs["distCoeffs"] >> camDistCoeffs;
	
	FileStorage pfs(ofToDataPath("pro.yml"), FileStorage::READ);
	pfs["cameraMatrix"] >> proMatrix;
	proSize.width = pfs["imageSize_width"];
	proSize.height = pfs["imageSize_height"];
	pfs["distCoeffs"] >> proDistCoeffs;
	pfs["rotation"] >> rotation;
	pfs["translation"] >> translation;
	
	camImage.allocate(3456, 2304, OF_IMAGE_GRAYSCALE);
	proImage.allocate(1024, 768, OF_IMAGE_GRAYSCALE);
	
	getRemapPoints("skull/", proImage.getWidth(), proImage.getHeight(), camImagePoints, proImagePoints, colors, GRAYCODE_MODE_OPPOSITES);
	getProCamImages("skull/", proImage, camImage, proImage.getWidth(), proImage.getHeight(), GRAYCODE_MODE_OPPOSITES);
	camImage.update();
	proImage.update();
	
	camImage.saveImage("camImage.png");
	proImage.saveImage("proImage.png");
	
	proDistCoeffs = Mat::zeros(proDistCoeffs.size(), proDistCoeffs.type());
	//camDistCoeffs = Mat::zeros(camDistCoeffs.size(), camDistCoeffs.type());
	
	resultPoints = triangulatePositions(
		camImagePoints, camMatrix, camDistCoeffs,
		proImagePoints, proMatrix, proDistCoeffs,
		rotation, translation);
		
	mesh = drawObjectPoints(resultPoints);
	for(int i = 0; i < colors.size(); i++) {
		mesh.addColor(ofColor(colors[i]));
	}
	
	proCalibration.setup(proMatrix, proSize);
	camCalibration.setup(camMatrix, camSize);
}

void testApp::update() {
	ofFile fragFile("shader.frag"), vertFile("shader.vert");
	Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
	Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
	if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
		shader.load("shader");
	}
	lastFragTimestamp = fragTimestamp;
	lastVertTimestamp = vertTimestamp;
}

void testApp::draw() {
	ofBackground(0);
	
	if(cameraMode == EASYCAM_MODE) {
		cam.begin();
		ofScale(1, -1, -1);
	} else if(cameraMode == PRO_MODE) {
		ofSetupScreenPerspective(proSize.width, proSize.height);
		proCalibration.loadProjectionMatrix();
		
		// build and invert extrinsics
		Mat r = rotation;
		Mat t = translation;
		Mat extrinsics = (Mat1d(4,4) <<
			r.at<double>(0,0), r.at<double>(0,1), r.at<double>(0,2), t.at<double>(0),
			r.at<double>(1,0), r.at<double>(1,1), r.at<double>(1,2), t.at<double>(1),
			r.at<double>(2,0), r.at<double>(2,1), r.at<double>(2,2), t.at<double>(2),
			0, 0, 0, 1);
		extrinsics = extrinsics.inv().t();
		glMultMatrixd((GLdouble*) extrinsics.ptr(0,0));
		
	} else if(cameraMode == CAM_MODE) {
		ofSetupScreenPerspective(camSize.width, camSize.height);
		camCalibration.loadProjectionMatrix();
	}
	
	glEnable(GL_DEPTH_TEST);
	
	if(cameraMode == EASYCAM_MODE) {
		ofTranslate(0, 0, -1200);
		ofSetColor(magentaPrint);
		drawCamera("camera", camMatrix, camSize, camImage);
		drawCamera("projector", proMatrix, proSize, proImage, rotation, translation);
	}
		
	ofSetColor(255);
	
	shader.begin();
	shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
	shader.setUniform1f("mouseX", mouseX);
	shader.setUniform1f("mouseY", mouseY);
	mesh.draw();
	shader.end();
	
	if(cameraMode == EASYCAM_MODE) {
		cam.end();
	}
}

void testApp::keyPressed(int key) {
	switch(key) {
		case '1': cameraMode = EASYCAM_MODE; break;
		case '2': cameraMode = PRO_MODE; break;
		case '3': cameraMode = CAM_MODE; break;
	}
}