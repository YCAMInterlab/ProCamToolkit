#pragma once

#include "Scene.h"

class Wind {
public:
	static const float speed = 100; // pixels / second
	static const float recovery = .01; // pixels / second
	ofColor c;
	ofVec2f f, v, p, o, prev;
	void addForce(ofVec2f& source, float magnitude) {
		ofVec2f direction = source - p;
		float length = direction.length();
		direction.normalize();
		direction *= magnitude / length;
		f += direction;
	}
	void addCenteringForce() {
		f.x += 1;
		f.y += (o.y - p.y) * recovery;
	}
	void update(float dt) {
		addCenteringForce();
		v += f * dt;
		v.normalize();
		prev = p;
		p += v * speed * dt;
		f.set(0);
	}
	void mirror(float width) {
		if(p.x >= width) {p.x = 1;}
		if(p.x <= 0) {p.x = width - 1;}
	}
};

class WindScene : public Scene {
public:
	bool clear, circles;
	vector<Wind> wind;
	void setup() {
		clear = false;
		circles = true;	
		int n = 40;
		wind.resize(n);
		for(int i = 0; i < n; i++) {
			float x = 1;
			float y = ofMap(i, -1, n, 0, height);
			wind[i].c.set(i%2 ? 0 : 255);
			wind[i].p.set(x, y);
			wind[i].o.set(x, y);
		}
	}
	void update() {
		float dt = ofGetLastFrameTime();
		for(int i = 0; i < wind.size(); i++) {
			for(int j = 0; j < touches.size(); j++) {
				wind[i].addForce(touches[j], -100);
			}
			wind[i].update(dt);
			wind[i].mirror(width);
		}
	}
	void draw() {
		glDisable(GL_DEPTH_TEST);
		
		if(clear) {
			ofEnableAlphaBlending();
			ofSetColor(0, 10);
			ofRect(0, 0, width, height);
			ofDisableAlphaBlending();
		}
		
		if(circles) {
			setPointSize(height / wind.size());
			ofMesh mesh;
			for(int i = 0; i < wind.size() - 1; i++) {
				mesh.addColor(wind[i].c);
				mesh.addVertex(wind[i].p);
			}
			mesh.drawVertices();
			setPointSize(0);
		} else {
			for(int i = 0; i < wind.size() - 1; i++) {
				ofVec2f nw = wind[i].prev;
				ofVec2f ne = wind[i].p;
				ofVec3f se = wind[i+1].p;
				ofSetColor(wind[i].c);
				if(nw.x <= ne.x) {
					ofRect(nw.x, nw.y, ne.x - nw.x, se.y - nw.y);
				}
			}		
		}
	}
};