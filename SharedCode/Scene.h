#pragma once

#include "ofMain.h"

class Scene {
public:
	void setSize(int width, int height) {
		this->width = width;
		this->height = height;
	}
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void reload() {
	}
	void setTouches(vector<ofVec2f>& touches) {
		this->touches = touches;
	}
protected:
	vector<ofVec2f> touches;
	int width, height;
	
	ofVec2f getAverageTouch() {
		ofVec2f avg;
		for(int i = 0; i < touches.size(); i++) {
			avg += touches[i];
		}
		avg /= touches.size();
		return avg;
	}
	void setPointSize(int x) {
		if(x == 0) {
			glPointSize(0);
			glDisable(GL_POINT_SMOOTH);
		} else {
			glEnable(GL_POINT_SMOOTH);
			glPointSize(x);
		}
	}
};

inline void regularPolygon(ofVec2f position, float outerRadius, int order = 4) {
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 0; i <= order; i++) {
		float curAngle = ofMap(i, 0, order, 0, TWO_PI);
		ofVec2f offset(cos(curAngle), sin(curAngle));
		mesh.addVertex(position);
		mesh.addVertex(position + offset * outerRadius);
	}
	mesh.drawFaces();
}

static ofMesh texturedRectMesh;
inline void texturedRect(float width, float height) {
	if(texturedRectMesh.getNumVertices() == 0) {
		texturedRectMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		texturedRectMesh.addTexCoord(ofVec2f(0, 0));
		texturedRectMesh.addVertex(ofVec2f(0, 0));
		texturedRectMesh.addTexCoord(ofVec2f(0, 1));
		texturedRectMesh.addVertex(ofVec2f(0, 1));
		texturedRectMesh.addTexCoord(ofVec2f(1, 0));
		texturedRectMesh.addVertex(ofVec2f(1, 0));
		texturedRectMesh.addTexCoord(ofVec2f(1, 1));
		texturedRectMesh.addVertex(ofVec2f(1, 1));
	}
	ofPushMatrix();
	ofScale(width, height);
	texturedRectMesh.drawFaces();
	ofPopMatrix();
}