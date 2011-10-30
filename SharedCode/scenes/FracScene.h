#pragma once

#include "Scene.h"

static bool drawNodePolygon;

class Node {
protected:
	const static float maxAge = 4;
	const static unsigned char maxLevels = 6;
	
	void draw(Node* node, bool black) {
		if(node == NULL) {
			ofSetColor(black ? ofColor::black : ofColor::white);
			if(drawNodePolygon) {
				regularPolygon(ofVec2f(.5, .5), .5, 3 + level);
			} else {
				ofRect(0, 0, 1, 1);
			}
		} else {
			node->draw(black);
		}
	}
	void touch(Node*& node, ofVec2f pos) {
		if(pos.x > 0 && pos.x < 1 && pos.y > 0 && pos.y < 1) {
			if(node == NULL) {
				node = new Node(level + 1);
			} else {
				node->touch(pos);
			}
		}
	}
	void prune(Node*& node, float time) {
		if(node != NULL) {
			if(node->presence) {
				node->birth = time;
				node->prune(time);
			} else {
				float age = time - node->birth;
				if(age > maxAge) {
					delete node;
					node = NULL;
				} else {
					node->prune(time);
				}
			}
		}
	}
	void resetPresence(Node*& node) {
		if(node != NULL) {
			node->resetPresence();
		}
	}
public:
	unsigned char level;
	Node *nw, *ne, *sw, *se;
	float birth;
	bool presence;
	
	Node(unsigned char level = 0)
	:level(level)
	,birth(ofGetElapsedTimef())
	,presence(false)
	,nw(NULL)
	,ne(NULL)
	,sw(NULL)
	,se(NULL) {
	}
	~Node() {
		if(nw != NULL) {
			delete nw;
		}
		if(ne != NULL) {
			delete ne;
		}
		if(sw != NULL) {
			delete sw;
		}
		if(se != NULL) {
			delete se;
		}
	}
	void draw(bool black = true) {
		ofPushMatrix();
		ofScale(.5, .5);
		draw(nw, black);
		ofTranslate(1, 0);
		draw(ne, !black);
		ofTranslate(-1, 1);
		draw(sw, !black);
		ofTranslate(1, 0);
		draw(se, black);
		ofPopMatrix();
	}
	void touch(ofVec2f pos) {
		presence = true;
		if(level < maxLevels) {
			pos *= 2;
			touch(nw, pos - ofVec2f(0, 0));
			touch(ne, pos - ofVec2f(1, 0));
			touch(sw, pos - ofVec2f(0, 1));
			touch(se, pos - ofVec2f(1, 1));
		}
	}
	void prune(float time) {
		prune(nw, time);
		prune(ne, time);
		prune(sw, time);
		prune(se, time);
	}
	void resetPresence() {
		presence = false;
		resetPresence(nw);
		resetPresence(ne);
		resetPresence(sw);
		resetPresence(se);
	}
};

class FracScene : public Scene {
public:
	Node node;
	void setup() {
		drawNodePolygon = true;
	}
	void update() {
		node.resetPresence();
		for(int i = 0; i < touches.size(); i++) {
			ofVec2f cur = touches[i];
			cur.x /= width;
			cur.y /= height;
			node.touch(cur);
		}
		node.prune(ofGetElapsedTimef());
	}
	void draw() {
		ofPushStyle();
		ofPushMatrix();
		ofClear(128, 255);
		ofScale(width, height);
		node.draw();
		ofPopMatrix();
		ofPopStyle();
	}
};