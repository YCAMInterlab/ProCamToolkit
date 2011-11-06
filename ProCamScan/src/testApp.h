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
	
	ofImage camImage, proImage;
	vector<Point2f> camImagePoints, proImagePoints;
	vector<Point3f> resultPoints;
	cv::Size proSize, camSize;
	Mat camMatrix, proMatrix;
	Mat camDistCoeffs, proDistCoeffs;
	Mat rotation, translation;
	ofEasyCam cam;
	ofVboMesh mesh;
	vector<unsigned char> colors;
};
