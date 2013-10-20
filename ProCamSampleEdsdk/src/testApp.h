#pragma once

#include "ofMain.h"

#include "ofxEdsdk.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	const static int tw = 512 * 3;
	const static int th = 256;
	
	ofxEdsdk::Camera camera;
};
