#pragma once

#include "ofMain.h"
#include "ofxLibdc.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	// ofxLibdc is a namespace, and Camera is the object
	ofxLibdc::PointGrey camera;	
	
	// ofImage is where we store the current frame we grabbed from the Camera 
	ofImage curFrame;
	bool saveNext;
};
