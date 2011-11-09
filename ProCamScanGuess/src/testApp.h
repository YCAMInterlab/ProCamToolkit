#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void decodeAndSave(string filename);
	void keyPressed(int key);
	
	ofImage camImage, proImage;
	vector<cv::Point2f> camImagePoints, proImagePoints;
	vector<cv::Point3f> resultPoints;
	cv::Size proSize, camSize;
	cv::Mat camMatrix, proMatrix;
	cv::Mat camDistCoeffs, proDistCoeffs;
	cv::Mat rotation, translation;
	ofEasyCam cam;
	ofVboMesh mesh;
	vector<unsigned char> colors;
	
	ofxAutoControlPanel panel;
};
