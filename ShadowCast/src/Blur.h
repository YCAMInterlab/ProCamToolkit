#pragma once

#include "ofMain.h"

class Blur {
private:
	ofFbo ping, pong;
	ofShader horizontalBlur, verticalBlur;
	int passes;
	float radius;
	
public:
	void setup(int width, int height) {
		ping.allocate(width, height);
		pong.allocate(width, height);
		ping.begin(); ofClear(0); ping.end();
		pong.begin(); ofClear(0); pong.end();		
		horizontalBlur.load("", "shaders/horizontalBlur.frag");
		verticalBlur.load("", "shaders/verticalBlur.frag");
	}
	void setPasses(int passes) {
		this->passes = passes;
	}
	void setRadius(float radius) {
		this->radius = radius;
	}
	void begin() {
		ofPushStyle();
		ofPushMatrix();
		pong.begin();
	}
	void end(bool draw = true) {
		pong.end();
		
		glColor4f(1, 1, 1, 1);
		for(int i = 0; i < passes; i++) {	
			ping.begin();
			horizontalBlur.begin();
			horizontalBlur.setUniform1f("radius", radius);
			pong.draw(0, 0);
			horizontalBlur.end();
			ping.end();
			
			pong.begin();
			verticalBlur.begin();
			verticalBlur.setUniform1f("radius", radius);
			ping.draw(0, 0);
			verticalBlur.end();
			pong.end();
		}
		
		if(draw) {
			pong.draw(0, 0);
		}
		ofPopStyle();
		ofPopMatrix();
	}
	ofTexture& getTextureReference() {
		return pong.getTextureReference();
	}
};