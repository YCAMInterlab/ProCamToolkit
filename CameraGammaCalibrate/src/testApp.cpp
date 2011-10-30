#include "testApp.h"

void testApp::setup() {
	solver.setup();
	ofBackground(255, 255, 255);
}

void testApp::update() {
}

void testApp::draw() {
	if(solver.ready) {
		int width = (int) ofGetWidth();
		int height = (int) ofGetHeight();
		int levels = solver.levels;

		if(solver.type == OF_IMAGE_COLOR) {
			ofSetColor(255, 0, 0);
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < levels; i++)
				glVertex2f(i, height - solver.redGamma[i]);
			glEnd();

			ofSetColor(0, 255, 0);
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < levels; i++)
				glVertex2f(i, height - solver.greenGamma[i]);
			glEnd();

			ofSetColor(0, 0, 255);
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < levels; i++)
				glVertex2f(i, height - solver.blueGamma[i]);
			glEnd();
		} else if(solver.type == OF_IMAGE_GRAYSCALE) {
			ofSetColor(0, 0, 0);
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < levels; i++)
				glVertex2f(i, height - solver.grayGamma[i]);
			glEnd();
		}
	}
}
