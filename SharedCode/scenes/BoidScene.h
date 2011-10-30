#pragma once

#include "Scene.h"
#include "NoiseParticle.h"

class BoidScene : public Scene {
public:
	void setup() {
		NoiseParticle::setup(100, 250);
	}
	void update() {
		float dt = ofGetLastFrameTime();
		ofVec2f offset(width / 2, height / 2);
		for(int i = 0; i < touches.size(); i++) {
			NoiseParticle::addForce(touches[i] - offset, 80);
		}
		NoiseParticle::updateAll(40 * dt);
	}
	void draw() {		
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(width / 2, height / 2);
		
		setPointSize(16);
		
		int n = NoiseParticle::size();
		
		// draw half white
		ofSetColor(255);
		NoiseParticle::drawRange(0, n / 2);
		// draw half black
		ofSetColor(0);
		NoiseParticle::drawRange(n / 2, n);
		
		setPointSize(0);
		
		ofPopMatrix();
		ofPopStyle();
	}
};