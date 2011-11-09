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
	void keyPressed(int key);
	
	cv::Size patternSize;
	float squareSize;
	int curPair;
	vector<ofImage> camImages, proImages;
	vector<vector<cv::Point2f> > camImagePoints, proImagePoints;
	vector<vector<cv::Point3f> > objectPoints;
	
	vector<vector<cv::Point3f> > resultPoints;
	
	cv::Size proSize, camSize;
	
	cv::Mat camMatrix, proMatrix;
	cv::Mat camDistCoeffs, proDistCoeffs;
	vector<cv::Mat> camRot, camTrans, proRot, proTrans;
	
	cv::Mat rotation, translation;
	cv::Mat fundamentalMatrix, essentialMatrix;
	
	ofEasyCam cam;
};
