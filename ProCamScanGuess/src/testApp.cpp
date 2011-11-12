#include "testApp.h"

using namespace cv;
using namespace ofxCv;

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetDataPathRoot("../../../../../SharedData/");
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
	panel.setup();
	panel.addPanel("settings");
	panel.addSlider("r0", -.12, -HALF_PI, +HALF_PI);
	panel.addSlider("r1", +.21, -HALF_PI, +HALF_PI);
	panel.addSlider("r2", 0, -HALF_PI, +HALF_PI);
	panel.addSlider("tx", -2500, -5000, +5000);
	panel.addSlider("ty", 0, -1000, +1000);
	panel.addSlider("tz", 0, -2000, +2000);
	panel.addToggle("update", true);
	panel.addSlider("zoffset", -1000, -10000, +10000);

	FileStorage cfs(ofToDataPath("cam-calibration.yml"), FileStorage::READ);
	cfs["cameraMatrix"] >> camMatrix;
	camSize.width = cfs["imageSize_width"];
	camSize.height = cfs["imageSize_height"];
	cfs["distCoeffs"] >> camDistCoeffs;
	
	FileStorage pfs(ofToDataPath("pro-calibration.yml"), FileStorage::READ);
	pfs["cameraMatrix"] >> proMatrix;
	proSize.width = pfs["imageSize_width"];
	proSize.height = pfs["imageSize_height"];
	pfs["distCoeffs"] >> proDistCoeffs;
	
	camImage.allocate(3456, 2304, OF_IMAGE_GRAYSCALE);
	proImage.allocate(1024, 768, OF_IMAGE_GRAYSCALE);
	
	getRemapPoints("scan/", proImage.getWidth(), proImage.getHeight(), camImagePoints, proImagePoints, colors, GRAYCODE_MODE_GRAY);
	getProCamImages("scan/", proImage, camImage, proImage.getWidth(), proImage.getHeight(), GRAYCODE_MODE_GRAY);
	camImage.update();
	proImage.update();
	
	// use zeros for dist coeffs. for some reason it's not right otherwise.
	proDistCoeffs = Mat::zeros(proDistCoeffs.size(), proDistCoeffs.type());
	camDistCoeffs = Mat::zeros(camDistCoeffs.size(), camDistCoeffs.type());
}

void testApp::update() {
	Mat rotationVector = (Mat1d(3, 1) << panel.getValueF("r0"), panel.getValueF("r1"), panel.getValueF("r2"));
	translation = (Mat1d(3, 1) << panel.getValueF("tx"), panel.getValueF("ty"), panel.getValueF("tz"));
	Rodrigues(rotationVector, rotation);
	
	if(panel.getValueB("update")) {
		panel.setValueB("update", false);
		
		resultPoints = triangulatePositions(
			camImagePoints, camMatrix, camDistCoeffs,
			proImagePoints, proMatrix, proDistCoeffs,
			rotation, translation);
		
		mesh = drawObjectPoints(resultPoints);
		for(int i = 0; i < colors.size(); i++) {
			mesh.addColor(ofColor(colors[i]));
		}
		
		cout << "rotation: " << rotation << endl;
		cout << "translation: " << translation << endl;
		
		exportPlyCloud("output.ply", mesh);
	}
}

void testApp::draw() {
	ofPushMatrix();
	ofBackground(0);
	
	cam.setNearClip(1);
	cam.setFarClip(100000);
	cam.begin();
	ofScale(1, -1, -1);
	
	glEnable(GL_DEPTH_TEST);
	
	ofTranslate(0, 0, panel.getValueF("zoffset"));
	
	ofSetColor(magentaPrint);
	drawCamera("camera", camMatrix, camSize, camImage);
	drawCamera("projector", proMatrix, proSize, proImage, rotation, translation);
		
	ofSetColor(255);
	mesh.draw();
	
	cam.end();
	
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
}

void testApp::keyPressed(int key) {
}