#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxProCamToolkit.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void decodeAndSave(string filename);
	
	cv::Size patternSize;
	
	ofImage camImage, proImage;
	vector<Point2f> camCenters, proCenters;
};
