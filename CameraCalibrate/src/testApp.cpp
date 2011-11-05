#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
	FileStorage settings(ofToDataPath("settings.yml"), FileStorage::READ);
	if(settings.isOpened()) {
		int xCount = settings["xCount"], yCount = settings["yCount"];
		calibration.setPatternSize(xCount, yCount);
		float squareSize = settings["squareSize"];
		calibration.setSquareSize(squareSize);
		CalibrationPattern patternType;
		switch(settings["patternType"]) {
			case 0: patternType = CHESSBOARD; break;
			case 1: patternType = CIRCLES_GRID; break;
			case 2: patternType = ASYMMETRIC_CIRCLES_GRID; break;
		}
		calibration.setPatternType(patternType);
	}
	
	calibration.calibrateFromDirectory("images");
	calibration.clean();
	calibration.save("calibration.yml");
}

void testApp::update() {
}

void testApp::draw() {
	ofBackground(255);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofTranslate(0, 0, -500); // center drawing at 500 mm
	ofDrawAxis(100);
	calibration.draw3d();
	cam.end();
	
	// draw info
	stringstream intrinsics, distcoeffs;
	intrinsics << "fov: " << toOf(calibration.getDistortedIntrinsics().getFov());
	drawHighlightString(intrinsics.str(), 10, 20, yellowPrint, ofColor(0));
	distcoeffs << "distCoeffs: " << calibration.getDistCoeffs();
	drawHighlightString(distcoeffs.str(), 10, 40, yellowPrint, ofColor(0));
	drawHighlightString("reproj error: " + ofToString(calibration.getReprojectionError()) + " from " + ofToString(calibration.size()), 10, 60, magentaPrint);
	for(int i = 0; i < calibration.size(); i++) {
		drawHighlightString(ofToString(i) + ": " + ofToString(calibration.getReprojectionError(i)), 10, 80 + 16 * i, magentaPrint);
	}
}

void testApp::keyPressed(int key) {
}
