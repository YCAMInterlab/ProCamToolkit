#pragma once

#include "Scene.h"

class RectScene : public Scene {
public:
	void regularDonut(ofVec2f position, float outerRadius, float innerRadius, int order = 4) {
		ofMesh mesh;
		mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		for(int i = 0; i <= order; i++) {
			float curAngle = ofMap(i, 0, order, 0, TWO_PI);
			ofVec2f offset(cos(curAngle), sin(curAngle));
			mesh.addVertex(position + offset * innerRadius);
			mesh.addVertex(position + offset * outerRadius);
		}
		mesh.drawFaces();
	}
	float maxRadius;
	int n, orderMultiplier, orderOffset;
	void setup() {
		maxRadius = width + height;
		n = 50;
		orderOffset = 5;
		orderMultiplier = 1;
	}
	void update() {
		touches.push_back(ofVec2f(0, 0));
		touches.push_back(ofVec2f(0, height));
		touches.push_back(ofVec2f(width, height));
		touches.push_back(ofVec2f(width, 0));
	}
	void draw() {
		float curveMin = .1;
		float curveMax = 10;
		// main computational limit is fill rate
		// to make it faster, write a square-donut
		vector<float> power;
		for(int j = 0; j < touches.size(); j++) {
			float distance = ofDist(touches[j].x, touches[j].y, width / 2, height / 2);
			float cur = ofMap(distance, 0, maxRadius / 2, curveMin, curveMax);
			power.push_back(cur);
		}
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < touches.size(); j++) {
				float curr = powf(1. - (float) (i) / n, power[j]) * maxRadius;
				float nextr = powf(1. - (float) (i + 1) / n, power[j]) * maxRadius;
				ofSetColor(i % 2 == 0 ? 0 : 255);
				regularDonut(touches[j], curr, nextr, orderOffset + j * orderMultiplier);
			}
		}
	}
};