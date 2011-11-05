#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	//camera.setFormat7(true);
	camera.setup();
	//camera.setMaxFramerate();
	camera.setBrightness(0);
	camera.setGain(0);
	camera.setExposure(1);
	camera.setGammaAbs(1);
	camera.setShutterAbs(1. / 30.);
	camera.printFeatures();
	ofSetWindowShape(camera.getWidth(), camera.getHeight());
}

void testApp::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
		if(saveNext) {
			curFrame.saveImage("out.png");
			saveNext = false;
		}
	}
}

void testApp::draw() {
	if(camera.isReady()) {
		curFrame.draw(0, 0);
	}
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		camera.setBlocking(true);
		int n = 32;
		for(int i = 0; i < n; i++) {
			camera.setShutter((float) i / n);
			camera.flushBuffer();
			ofSleepMillis(50);
			camera.grabStill(curFrame);
			float cur = camera.getShutterAbs() * 1000 * 1000;
			curFrame.saveImage(ofToString((int) cur) + ".png");
		}
		camera.setBlocking(false);
	}
	if(key == '\t') {
		saveNext = true;
	}
}