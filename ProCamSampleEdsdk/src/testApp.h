#pragma once

#include "ofMain.h"

#include "ofxEdsdk.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	const static int tw = 1024;
	const static int th = 768;
	
	ofxEdsdk::Camera camera;
};
