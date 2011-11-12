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
	
	enum CameraMode {EASYCAM_MODE, PRO_MODE, CAM_MODE};
	CameraMode cameraMode;
	
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
	
	ofShader shader;
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofxCv::Intrinsics proCalibration, camCalibration;
};
