#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxProCamToolkit.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void decodeAndSave(int i, string filename);
	
	static const int
		pw = 1024, ph = 768,
		cw = 3456, ch = 2304;
	
	ofImage camImage, proImage;
};
