#include "BinnedParticle.h"

#include "ofGraphics.h"

BinnedParticle::BinnedParticle(float x, float y, float xv, float yv) :
	x(x), y(y),
	xv(xv), yv(yv) {
}

void BinnedParticle::updatePosition(float timeStep) {
	// f = ma, m = 1, f = a, v = int(a)
	xv += xf * timeStep;
	yv += yf * timeStep;
	x += xv * timeStep;
	y += yv * timeStep;
}

void BinnedParticle::resetForce() {
	xf = 0;
	yf = 0;
}

void BinnedParticle::bounceOffWalls(float left, float top, float right, float bottom, float damping) {
	bool collision = false;

	if (x > right){
		x = right;
		xv *= -1;
		collision = true;
	} else if (x < left){
		x = left;
		xv *= -1;
		collision = true;
	}

	if (y > bottom){
		y = bottom;
		yv *= -1;
		collision = true;
	} else if (y < top){
		y = top;
		yv *= -1;
		collision = true;
	}

	if (collision == true){
		xv *= damping;
		yv *= damping;
	}
}

void BinnedParticle::addDampingForce(float damping) {
	xf -= xv * damping;
	yf -= yv * damping;
}

void BinnedParticle::draw() {
	glVertex2f(x, y);
}