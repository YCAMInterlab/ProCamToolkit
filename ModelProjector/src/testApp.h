#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "ofxLaunchpadToggle.h"

class testApp : public ofBaseApp, public ofxLaunchpadListener {
public:
	void setb(string name, bool value);
	void seti(string name, int value);
	void setf(string name, float value);
	bool getb(string name);
	int geti(string name);
	float getf(string name);
	
	void setup();
	void update();
	void draw();	
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	void setupControlPanel();
	void setupMesh();
	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
	void saveData();
	
	ofxAssimpModelLoader model;	
	ofEasyCam cam;
	ofMesh objectMesh, imageMesh;
	ofLight light;
	ofxAutoControlPanel panel;
	
	vector<cv::Point3f> objectPoints;
	vector<cv::Point2f> imagePoints;
	vector<bool> referencePoints;
	
	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;

	ofxLaunchpadToggle launchpad;
	ofxMidiOut midiOut;
	void gridButtonPressed(int col, int row);
	void gridButtonReleased(int col, int row);
	
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
};
