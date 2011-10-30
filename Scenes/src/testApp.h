#pragma once

#include "ofMain.h"

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

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	vector<Scene*> scenes;
	int curScene;
};
