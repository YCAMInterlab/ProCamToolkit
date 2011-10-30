#pragma once

#include "Scene.h"

class DotsScene : public Scene {
public:
	bool rotate, clear, rect;

	int n;
	void setup() {
		clear = true;
		rotate = false;
		rect = false;
		
		n = 30;
		ofSetCircleResolution(64);
	}
	void update() {
	}
	void draw() {
		ofPushStyle();
		if(clear) {
			ofSetColor(128);
			ofRect(0, 0, width, height);
		}
	
		ofVec2f avg = getAverageTouch();
		ofSetColor(255);
		ofTranslate(width / 2, height / 2);
		if(rotate) {
			ofRotate(4 * ofGetElapsedTimef());
		}
		float r = ofMap(ofDist(avg.x, avg.y, width / 2, height / 2), 0, width, 1, n);
		ofScale(r, r);
		float er = width / n;
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				float y = ofMap(i, -1, n, -height / 2, height / 2);
				float x = ofMap(j, -1, n, -width / 2, width / 2);
				if(i & 1 ^ j & 1) {
					ofSetColor(255);
				} else {
					ofSetColor(0);
				}
				if(rect) {
					ofRect(x - er/4, y - er/4, er/2, er/2);
				} else {
					ofCircle(x, y, er / 3);
				}
			}
		}
		ofPopStyle();
	}
};