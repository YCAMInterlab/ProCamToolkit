#include "testApp.h"

void testApp::setup() {
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, 0);
	ofSetBackgroundAuto(false);
	//ofSetVerticalSync(true);
	scenes.push_back(new OrigScene());
	scenes.push_back(new GridScene());
	scenes.push_back(new LensScene());
	scenes.push_back(new BallScene());
	scenes.push_back(new VoroScene());
	scenes.push_back(new FracScene());
	scenes.push_back(new WindScene());
	scenes.push_back(new BoidScene());
	scenes.push_back(new DotsScene());
	scenes.push_back(new WallScene());
	scenes.push_back(new RectScene());
	for(int i = 0; i < scenes.size(); i++) {
		scenes[i]->setSize(ofGetWidth(), ofGetHeight());
		scenes[i]->setup();
	}
	curScene = 0;
}

void testApp::update() {
	vector<ofVec2f> touches;
	ofSeedRandom(0);
	for(int i = 0; i < 1; i++) {
		float f = .1;
		touches.push_back(
			ofVec2f(
				ofGetWidth() * ofNoise(ofGetElapsedTimef() * f + ofRandom(1024)),
				ofGetHeight() * ofNoise(ofGetElapsedTimef() * f + ofRandom(1024))));
	}
	if(ofGetMousePressed()) {
		touches.push_back(ofVec2f(mouseX, mouseY));
	}
	scenes[curScene]->setTouches(touches);
	scenes[curScene]->update();
}

void testApp::draw() {	
	ofPushMatrix();
	//ofPushStyle();
	scenes[curScene]->draw();
	//ofPopStyle();
	ofPopMatrix();
	
	if(ofGetMousePressed()) {
		int frameLength = 1000 * ofGetLastFrameTime();
		ofSetColor(frameLength > 10 ? ofColor::red : ofColor::green);
		ofDrawBitmapString(ofToString(frameLength) + " ms", 10, 20);
	}
}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_UP) {
		curScene--;
	}
	if(key == OF_KEY_DOWN) {
		curScene++;
	}
	curScene = ofClamp(curScene, 0, scenes.size() - 1);
}