#pragma once

#include "ofMain.h"

class BinnedParticle {
public:
	float x, y;
	float xv, yv;
	float xf, yf;
	BinnedParticle(float x = 0, float y = 0, float xv = 0, float yv = 0);
	void updatePosition(float timeStep);
	void resetForce();
	void bounceOffWalls(float left, float top, float right, float bottom, float damping = .3);
	void addDampingForce(float damping = .01);
	void draw();
};
