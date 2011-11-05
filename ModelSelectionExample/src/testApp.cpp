#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
	model.loadModel("head.ply");
	mesh = model.getMesh(0);
}

void testApp::update() {
}

void testApp::draw() {
	ofBackground(0);
	ofSetColor(255);
	
	cam.begin();
	ofScale(.5, .5, .5);
	ofSetLineWidth(1);
	mesh.drawWireframe();
	ofMesh screen = getProjectedMesh(mesh);	
	cam.end();
	
	glPointSize(5);
	glEnable(GL_POINT_SMOOTH);
	ofSetColor(cyanPrint);
	screen.drawVertices();
	
	int best;
	ofVec3f choice = getClosestPointOnMesh(screen, mouseX, mouseY, &best);
	ofSetColor(magentaPrint);
	ofNoFill();
	ofSetLineWidth(4);
	ofCircle(choice, 8);
}
