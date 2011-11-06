#pragma once

#include "ofMain.h"

class Remap {
private:
	ofFloatImage remap;
	ofTexture* scene;
	ofTexture* shoji;
	ofFloatImage lut;
	ofFbo result;
	ofShader remapShader;
	float weight;
	float scale;
	ofVec2f offset;
	
public:
	
	void setup(string remapFilename, string lutFilename) {
		remap.loadImage(remapFilename);
		
		//lut.loadImage(lutFilename);
		lut.allocate(256, 1, OF_IMAGE_COLOR);
		ofFile lutFile(lutFilename);
		ofBuffer lutBuffer = lutFile.readToBuffer();
		int x = 0;
		while(!lutBuffer.isLastLine()) {
			string curLine = lutBuffer.getNextLine();
			vector<string> parts = ofSplitString(curLine, "\t");
			lut.setColor(x, 0, ofFloatColor(ofToFloat(parts[0]), ofToFloat(parts[1]), ofToFloat(parts[2])));
			x++;
		}
		lut.update();
		lut.saveImage(lutFilename + ".exr");
		
		result.allocate(remap.getWidth(), remap.getHeight(), GL_RGBA); // doesn't need a depth buffer
		remapShader.load("", "shaders/remap.frag");
	}
	void setScale(float scale) {
		this->scale = scale;
	}
	void setOffset(ofVec2f offset) {
		this->offset = offset;
	}
	void setScene(ofTexture& scene) {
		this->scene = &scene;
	}
	void setShoji(ofTexture& shoji, float weight) {
		this->shoji = &shoji;
		this->weight = weight;
	}
	void update() {
		ofPushMatrix();
		result.begin();
		remapShader.begin();
		remapShader.setUniformTexture("remap", remap.getTextureReference(), 1);
		remapShader.setUniformTexture("scene", *scene, 2);
		remapShader.setUniformTexture("shoji", *shoji, 3);
		remapShader.setUniformTexture("lut", lut.getTextureReference(), 4);
		remapShader.setUniform1f("weight", weight);
		remapShader.setUniform2f("sceneOffset", offset.x, offset.y);
		remapShader.setUniform1f("sceneScale", scale);
		remap.draw(0, 0);
		remapShader.end();
		result.end();
		ofPopMatrix();
	}
	ofTexture& getTextureReference() {
		return result.getTextureReference();
	}
};