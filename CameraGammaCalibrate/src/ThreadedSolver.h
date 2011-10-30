#pragma once

#include "ofMain.h"
#include "GammaCalibration.h"
#include "ofxDirList.h"
#include "LutFilter.h"
#include "ofxThread.h"
#include <fstream>

// use DOUBLECHECK to load back in the LUT and see how linear the results are
//#define DOUBLECHECK

class ThreadedSolver : public ofxThread {
public:
	ThreadedSolver();

	void setup();

	bool ready;

	LutFilter lut;

	vector<ofImage> images;
	vector<float> shutterTimes;
	string directory;
	int sampledLocations;
	float smoothness;

	const static int levels = 256;
	int iterations;

	int type;

	vector<float> grayGammaSum;
	vector<float> grayGamma;

	vector<float> redGammaSum, greenGammaSum, blueGammaSum;
	vector<float> redGamma, greenGamma, blueGamma;

protected:
	void threadedFunction();
};
