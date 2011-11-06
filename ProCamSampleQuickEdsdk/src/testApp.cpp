#include "testApp.h"

#include "GrayCodeGenerator.h"
GrayCodeGenerator generator;
bool capturing = false;
int totalDirection = 2;
int totalStyles = 2;

int capture = 0;
int style = 0; // gray, patterns
int direction = 0; // false/true, 2 values
int pattern = 0; // 0-9, 10 values

string curDirectory;
long bufferTime = 120;
bool needToCapture = false;
long captureTime = 0;

void generate() {
	generator.setSize(testApp::tw, testApp::th);
	generator.setOrientation(direction == 0 ? PatternGenerator::VERTICAL : PatternGenerator::HORIZONTAL);
	generator.setInverse(false);
	generator.generate();
	
	stringstream dirStr;
	dirStr << capture << "/";
	if(style > 0) {
		dirStr <<	(direction == 0 ? "vertical/" : "horizontal/");
	}
	curDirectory = dirStr.str();
}

bool nextState() {
	if(style == 0) {
		style++;
		generate();
	} else {
		pattern++;
		if(pattern == generator.size()) {
			pattern = 0;
			direction++;
			if(direction == totalDirection) {
				direction = 0;
				style = 0;
				capture++;
				generate();
				return false;
			}
			generate();
		}
	}
	return true;
}

void testApp::setup() {
	ofSetVerticalSync(true);
	ofHideCursor();
	ofSetDataPathRoot("../../../../../SharedData/quick/");
	ofSetLogLevel(OF_LOG_VERBOSE);
	camera.setup();
	generate();
}

void testApp::update() {
	if(!capturing) {
		camera.update();
	}
	if(camera.isPhotoNew()) {
		ofDirectory::createDirectory(curDirectory, true, true);
		bool success = camera.savePhoto(curDirectory + ofToString(pattern) + ".jpg");
		if(nextState()) {
			captureTime = ofGetElapsedTimeMillis();
			needToCapture = true;
		} else {
			capturing = false;
		}
	}
}

void testApp::draw() {
	if(capturing) {
		ofBackground(128);
		if(style > 0) {
			generator.get(pattern).draw(0, 0);
		}
	} else {
		ofBackground(255);
		ofPushMatrix();
		ofTranslate(tw / 2, th / 2);
		ofSetColor(255);
		ofScale(.75, .75);
		ofTranslate(-camera.getLivePixels().getWidth() / 2, -camera.getLivePixels().getHeight() / 2);
		camera.draw(0, 0);
		ofPopMatrix();
	}
	if(needToCapture && (ofGetElapsedTimeMillis() - captureTime) > bufferTime) {
		camera.takePhoto();
		needToCapture = false;
	}
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		camera.takePhoto();
		capturing = true;
	}
	if(key == 'f') {
		ofToggleFullscreen();
	}
}