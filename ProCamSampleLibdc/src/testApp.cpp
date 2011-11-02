#include "testApp.h"

#include "GrayCodeGenerator.h"
GrayCodeGenerator generator;
bool capturing = false;
int totalProjectors = 1;
int totalDirection = 2;
int totalInverse = 2;

// every combination of these four properties
// for a total of: 10x2x2x2 = 80 images
int projector = 0;
int direction = 0;
int inverse = 0;
int pattern = 0;

string curDirectory;
long bufferTime = 500;
bool needToCapture = false;
unsigned long captureTime = 0;

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
	//ofSetDataPathRoot("../../../../../SharedData/");
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	camera.setup();
	camera.setBrightness(0);
	camera.setGain(0);
	camera.setExposure(1);
	camera.setGammaAbs(1);
	camera.setShutter(1);
	camera.printFeatures();
	
	generate();
}

void testApp::update() {
	unsigned long curTime = ofGetSystemTime();
	
	// instead of a fixed delay, we could just wait until a frame after we see a difference
	bool needToCapture = capturing && (curTime - captureTime) > bufferTime;
	if(camera.grabVideo(curFrame) && needToCapture) {
		curFrame.saveImage(curDirectory + ofToString(pattern) + ".png");
		if(nextState()) {
			captureTime = curTime;
		} else {
			stopCapture();
		}
	}
}

void testApp::draw() {
	ofBackground(0);
	generator.get(pattern).draw(projector * tw, 0);
	if(!capturing) {
		ofPushMatrix();
		//ofScale(.25, .25);
		curFrame.update();
		curFrame.draw(0, 0);
		ofPopMatrix();
	}
}

void testApp::startCapture() {
	capturing = true;
	captureTime = ofGetSystemTime();
}

void testApp::stopCapture() {
	capturing = false;
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		startCapture();
	}
	if(key == 'f') {
		ofToggleFullscreen();
	}
}