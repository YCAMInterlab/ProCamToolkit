#pragma once

#include "ofMain.h"

inline void randomize(ofVec2f& v) {
	v.x = ofRandomf();
	v.y = ofRandomf();
	v.normalize();
}

class Force {
public:
	ofVec2f position;
	float magnitude;
	Force(ofVec2f position, float magnitude) {
		this->position = position;
		this->magnitude = magnitude;
	}
};

class NoiseParticle {
public:
	static ofVec2f centeringForce, globalOffset, externalForce;
	static float speed, spread, viscosity, independence, rebirthRadius, neighborhood, confusion, indecision, age, globalDirection;
	static vector<NoiseParticle> particles;
	static vector<Force> forces;
	static void setup(int n, float radius);
  static void drawRange(int start, int end);
  static void drawAll();
	static void addForce(ofVec2f position, float magnitude);
  static void updateAll(float dt);
	static int size();

  ofVec2f position, velocity, force, localOffset;
  NoiseParticle(float radius) {
    randomize(localOffset);
  	randomize(position);
  	position *= radius;
  }
  inline void applyFlockingForce() {
		float basex = position.x / neighborhood;
		float basey = position.y / neighborhood;
    force.x +=
      ofSignedNoise(
				basex + globalOffset.x + localOffset.x * independence,
				basey);
		force.y +=
      ofSignedNoise(
				basex,
				basey + globalOffset.y  + localOffset.y * independence);
  }
  inline void applyViscosityForce() {
    force += velocity * -viscosity;
  }
  inline void applyCenteringForce() {
    centeringForce = position;
    float distanceToCenter = centeringForce.length();
    centeringForce.normalize();
    centeringForce *= -distanceToCenter / (spread * spread);
    force += centeringForce;
  }
	inline void applyExternalForce() {
		for(int i = 0; i < forces.size(); i++) {
			externalForce = position - forces[i].position;
			float distanceToForce = externalForce.length();
			externalForce.normalize();
			externalForce *= forces[i].magnitude / distanceToForce;
			force += externalForce;		
		}
	}
  inline void update(float dt) {
    force.set(0, 0);
    applyFlockingForce();
    applyViscosityForce();
		applyCenteringForce();
		applyExternalForce();
    velocity += force * dt; // mass = 1, acceleration = force
    position += velocity * speed * dt;
  }
};
