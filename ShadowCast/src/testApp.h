#pragma once

#include "ofMain.h"

#include "Remap.h"

#include "ofxBlur.h"
#include "ofxKinect.h"

#include "OrigScene.h"
#include "GridScene.h"
#include "LensScene.h"
#include "BallScene.h"
#include "VoroScene.h"
#include "FracScene.h"
#include "WindScene.h"
#include "BoidScene.h"
#include "WallScene.h"
#include "RectScene.h"
#include "DotsScene.h"

#include "ofxAutoControlPanel.h"

#include "ofxCv.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void findUserLocations();
	void keyPressed(int key);
	
	void pickNextScene();
	void setFade();
	void updateScene();
	
	void drawScene();
	void drawShoji();
	void drawOverlay();
	
	ofxAutoControlPanel panel;
	static const int tw = 1024;
	static const int th = 768;
	
	// has to match sceneSize in remap shader
	static const int sceneWidth = 1024;
	static const int sceneHeight = 1024;
	
	ofxBlur shojiBlur;
	ofFbo sceneFbo;
	Remap leftRemap, rightRemap;
	bool showDebug, whiteScene;
	
	vector<Scene*> scenes;
	int curScene;
	
	ofxKinect kinect;
	Mat kinectAccumulator, kinectAccumulator8u;
	ContourFinder contourFinder;
	ofImage filled;
	vector<ofVec3f> users;
	vector<unsigned int> userLabels;
	
	float startFadeTime;
	float presence, lastPresence;
	float fadeState;
};
