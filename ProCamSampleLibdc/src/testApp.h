#pragma once

#include "ofMain.h"

#include "ofxLibdc.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	void startCapture();
	void stopCapture();
	
	const static int tw = 1024;
	const static int th = 768;
	
	ofxLibdc::PointGrey camera;
	ofImage curFrame;
};
