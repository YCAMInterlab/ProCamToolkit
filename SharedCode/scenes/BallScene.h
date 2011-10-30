#pragma once

#include "Scene.h"
#include "BinnedParticleSystem.h"

class BallScene : public Scene {
public:
	const static int binPower = 5;
	const static int padding = 256;
	
	float particleRadius, particleRepulsion, touchRadius, touchRepulsion, centerAttraction;
	BinnedParticleSystem particleSystem;
	bool drawBalls;
	
	void setup() {
		particleSystem.setup(width + padding * 2, height + padding * 2, binPower);
		for(int i = 0; i < 1500; i++) {
			float x = ofRandom(0, particleSystem.getWidth());
			float y = ofRandom(0, particleSystem.getHeight());
			BinnedParticle particle(x, y, 0, 0);
			particleSystem.add(particle);
		}
		
		particleSystem.setTimeStep(100);
		particleRadius = 64;
		particleRepulsion = .5;
		centerAttraction = .01;
		touchRadius = 400;
		touchRepulsion = 1;
		drawBalls = true;
	}
	void update() {
	}
	void draw() {
		ofBackground(0);
		
		ofSetColor(255);
		
		ofPushMatrix();
		ofTranslate(-padding, -padding);
		
		if(!drawBalls) {
			ofSetLineWidth(8);
			glBegin(GL_LINES);
		}
		particleSystem.setupForces();
		for(int i = 0; i < particleSystem.size(); i++) {
			BinnedParticle& cur = particleSystem[i];
			particleSystem.addRepulsionForce(cur, particleRadius, particleRepulsion);
			cur.bounceOffWalls(0, 0, particleSystem.getWidth(), particleSystem.getHeight());
			cur.addDampingForce();
		}
		if(!drawBalls) {
			glEnd();
		}
		
		particleSystem.addAttractionForce(particleSystem.getWidth() / 2, particleSystem.getHeight() / 2, particleSystem.getWidth() * 100, centerAttraction);
		for(int i = 0; i < touches.size(); i++) {
			particleSystem.addRepulsionForce(touches[i].x + padding, touches[i].y + padding, touchRadius, touchRepulsion);
		}
		particleSystem.update(ofGetLastFrameTime());
		
		// draw all the particles
		if(drawBalls) {
			for(int i = 0; i < particleSystem.size(); i++) {
				ofCircle(particleSystem[i].x, particleSystem[i].y, particleRadius * .25);
			}
		}
		
		ofPopMatrix();
	}
};