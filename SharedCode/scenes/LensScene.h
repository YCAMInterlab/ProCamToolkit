#pragma once

#include "Scene.h"

class LensScene : public Scene {
public:
	static const int maxPoints = 256;
	ofFloatImage pointsImage;
	ofShader shader;
	void setup() {
		shader.load("", "shaders/lens.frag");
		pointsImage.allocate(maxPoints, 1, OF_IMAGE_COLOR);
	}
	void update() {
		float* pixels = pointsImage.getPixels();
		for(int i = 0; i < touches.size() && i < maxPoints; i++) {
			pixels[i * 3 + 0] = touches[i].x / width;
			pixels[i * 3 + 1] = touches[i].y / height;
			pixels[i * 3 + 2] = (float) i / (touches.size() - 1);
		}
		pointsImage.update();
	}
	void draw() {
		shader.begin();
		shader.setUniform1i("count", touches.size());
		shader.setUniformTexture("points", pointsImage.getTextureReference(), 0);
		texturedRect(width, height);
		shader.end();
	}
	void reload() {
		shader.load("", "shaders/lens.frag");
	}
};