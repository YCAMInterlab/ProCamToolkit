#pragma once

#include "ofMain.h"

#include "ofxCv.h"
using namespace cv;
using namespace ofxCv;

const int tw = 1024;
const int th = 768;
const int tn = tw * th;

#include "Lut.h"
#include "ofxAutoControlPanel.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void drawProjector();
	void draw();
	
	void keyPressed(int key);
	
	ofShortImage binaryCodedLeftX, binaryCodedLeftY;
	ofShortImage binaryCodedRightX, binaryCodedRightY;
	ofImage binaryCodedLeft, binaryCodedRight;
	
	ofFbo leftFbo, rightFbo;
	ofShader gammaShader;
	Lut leftLut, rightLut;
	
	ofxAutoControlPanel panel;
};
