#include "testApp.h"

#include "GrayCodeGenerator.h"
GrayCodeGenerator generator;
bool capturing = false;
int totalProjectors = 1;
int totalDirection = 2;
int totalInverse = 2;

// every combination of these four properties
// for a total of: 10x2x2x2 = 80 images
int projector = 0; // 0-1, 2 values
int direction = 0; // false/true, 2 values
int inverse = 0; // false/true, 2 values
int pattern = 0; // 0-9, 10 values

string curDirectory;
long bufferTime = 100;
bool needToCapture = false;
long captureTime = 0;

void generate() {
	generator.setSize(testApp::tw, testApp::th);
	generator.setOrientation(direction == 0 ? PatternGenerator::VERTICAL : PatternGenerator::HORIZONTAL);
	generator.setInverse(inverse == 0);
	generator.generate();
	stringstream dirStr;
	dirStr <<
		(projector == 0 ? "left/" : "right/") <<
		(direction == 0 ? "vertical/" : "horizontal/") <<
		(inverse == 0 ? "inverse/" : "normal/");
	curDirectory = dirStr.str();
	ofDirectory::createDirectory(curDirectory, true, true);
}

bool nextState() {
	pattern++;
	if(pattern == generator.size()) {
		pattern = 0;
		inverse++;
		if(inverse == totalInverse) {
			inverse = 0;
			direction++;
			if(direction == totalDirection) {
				direction = 0;
				projector++;
				if(projector == totalProjectors) {
					projector = 0;
					return false;
				}
			}
		}
		generate();
	}
	return true;
}

void testApp::setup() {
	ofSetVerticalSync(true);
	ofHideCursor();
	//ofSetDataPathRoot("../../../../../SharedData/skull/");
	ofSetLogLevel(OF_LOG_VERBOSE);
	camera.setup();
	generate();
}

void testApp::update() {
	if(!capturing) {
		camera.update();
	}
	if(camera.isPhotoNew()) {
		camera.savePhoto(curDirectory + ofToString(pattern) + ".jpg");
		if(nextState()) {
			captureTime = ofGetElapsedTimeMillis();
			needToCapture = true;
		} else {
			capturing = false;
		}
	}
}

void testApp::draw() {
	ofBackground(0);
	generator.get(pattern).draw(projector * tw, 0);
	if(!capturing) {
		ofPushMatrix();
		ofScale(.25, .25);
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