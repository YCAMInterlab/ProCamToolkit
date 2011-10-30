#pragma once

#include "Scene.h"

class WallScene : public Scene {
public:
	float period;
	
	void setup() {
		period = 2;
	}
	void update() {
	}
	void draw() {
		ofPushStyle();
		float curTime = ofGetElapsedTimef();
		bool direction = fmodf(curTime, 2 * period) > period;
		float offset = ofMap(fmodf(curTime, period), 0, period, 0, width);
		if(direction) {
			ofSetColor(0);
			ofRect(0, 0, width, height);
			ofSetColor(255);
			ofRect(offset - width, 0, width, height);
		} else {
			ofSetColor(255);
			ofRect(0, 0, width, height);
			ofSetColor(0);
			ofRect(0, offset - height, width, height);
		}
		ofPopStyle();
	}
};