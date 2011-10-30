#pragma once

#include "ofxOpenCv.h"
#include <fstream>

class GammaCalibration {
public:
	static void calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& grayGamma,
		int sampledLocations = 45, float smoothness = 1);

	static void calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& redGamma, vector<float>& greenGamma, vector<float>& blueGamma,
		int sampledLocations = 45, float smoothness = 1);

private:
	static const int levels = 256;

	static void calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& gamma,
		int offset, int step,
		int sampledLocations, float smoothness);
};
