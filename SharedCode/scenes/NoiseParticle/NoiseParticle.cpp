#include "NoiseParticle.h"

vector<Force>
	NoiseParticle::forces;
	
vector<NoiseParticle>
	NoiseParticle::particles;

ofVec2f
	NoiseParticle::centeringForce,
	NoiseParticle::globalOffset,
	NoiseParticle::externalForce;

float
	NoiseParticle::speed,
	NoiseParticle::spread,
	NoiseParticle::viscosity,
	NoiseParticle::independence,
	NoiseParticle::neighborhood,
	NoiseParticle::confusion,
	NoiseParticle::indecision,
	NoiseParticle::age,
	NoiseParticle::globalDirection;

void NoiseParticle::setup(int n, float radius) {
	globalOffset.set(1. / 3, 2. / 3);
	speed = 1;
  spread = 40;
  viscosity = .1;
  independence = .5;
  neighborhood = 100;
	confusion = .01;
	indecision = .01;
	age = 0;
	globalDirection = 0;
	
	for(int i = 0; i < n; i++) {
		NoiseParticle::particles.push_back(NoiseParticle(radius));
	}
}

void NoiseParticle::drawAll() {
	drawRange(0, particles.size());
}

void NoiseParticle::drawRange(int start, int end) {
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	for(int i = start; i < end; i++) {
		mesh.addVertex(particles[i].position);
	}
	mesh.drawVertices();
}

void NoiseParticle::updateAll(float dt = 1. / 60.) {
	for(int i = 0; i < particles.size(); i++) {
		particles[i].update(dt);
	}
	globalDirection += ofSignedNoise(indecision * age);
	ofVec2f direction(0, 1);
	direction.rotate(globalDirection);
	globalOffset += confusion * direction * dt;
	age += dt;
	forces.clear();
}

void NoiseParticle::addForce(ofVec2f position, float magnitude) {
	forces.push_back(Force(position, magnitude));
}

int NoiseParticle::size() {
	return particles.size();
}