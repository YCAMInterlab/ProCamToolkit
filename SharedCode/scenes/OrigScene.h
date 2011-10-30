#pragma once

#include "Scene.h"

class OrigScene : public Scene {
public:
	static const int maxPoints = 256;
	ofFloatImage pointsImage;
	ofShader shader;
	void setup() {
		shader.load("", "shaders/origami.frag");
		pointsImage.allocate(maxPoints, 1, OF_IMAGE_COLOR);
	}
	void update() {
		int n = touches.size();
		int m = 7;
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < m; j++) {
				ofVec2f offset(100, 0);
				float theta = ofMap(j, 0, m, 0, TWO_PI);
				offset.rotateRad(theta);
				touches.push_back(touches[i] + offset);
			}
		}
	
		touches.push_back(ofVec2f(0, 0));
		touches.push_back(ofVec2f(0, height));
		touches.push_back(ofVec2f(width, height));
		touches.push_back(ofVec2f(width, 0));
		
		float* pixels = pointsImage.getPixels();
		for(int i = 0; i < touches.size() && i < maxPoints; i++) {
			pixels[i * 3 + 0] = touches[i].x / width;
			pixels[i * 3 + 1] = touches[i].y / height;
		}
		pointsImage.update();
	}
	void draw() {
		shader.begin();
		shader.setUniform1i("count", touches.size());
		shader.setUniformTexture("points", pointsImage.getTextureReference(), 0);
		texturedRect(width, height);
		shader.end();
		/*
		ofSetColor(ofColor::red);
		for(int i = 0; i < touches.size(); i++) {
			ofCircle(touches[i], 4);
		}*/
	}
	void reload() {
		shader.load("", "shaders/origami.frag");
	}
};