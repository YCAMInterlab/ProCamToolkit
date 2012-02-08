#pragma once

#include "ofMain.h"

class Lut {
private:
	ofTexture* source;
	ofFloatImage lut;
	ofFbo result;
	
	ofShader lutShader;
	
public:
	void setupText(int width, int height, string filename) {
		result.allocate(width, height, GL_RGB); // doesn't need a depth buffer
		lutShader.load("", "shaders/lut.frag");
		
		lut.allocate(256, 1, OF_IMAGE_COLOR);
		ofFile lutFile(filename);
		ofBuffer lutBuffer = lutFile.readToBuffer();
		int x = 0;
		while(!lutBuffer.isLastLine()) {
			string curLine = lutBuffer.getNextLine();
			vector<string> parts = ofSplitString(curLine, "\t");
			lut.setColor(x, 0, ofFloatColor(ofToFloat(parts[0]), ofToFloat(parts[1]), ofToFloat(parts[2])));
			x++;
		}
		lut.update();
	}
	void setup(int width, int height, string filename) {
		result.allocate(width, height, GL_RGB); // doesn't need a depth buffer
		lutShader.load("", "shaders/lut.frag");
		lut.loadImage(filename);
	}
	void setSource(ofTexture& source) {
		this->source = &source;
	}
	void update() {
		ofPushMatrix();
		result.begin();
		lutShader.begin();
		lutShader.setUniformTexture("source", *source, 0);
		lutShader.setUniformTexture("lut", lut.getTextureReference(), 1);
		source->draw(0, 0);
		lutShader.end();
		result.end();
		ofPopMatrix();
	}
	ofTexture& getTextureReference() {
		return result.getTextureReference();
	}
};