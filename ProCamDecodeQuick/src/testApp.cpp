#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::decodeAndSave(int i, string filename) {
	cout << "loading " << filename << endl;

	camImage.allocate(cw, ch, OF_IMAGE_GRAYSCALE);
	proImage.allocate(pw, ph, OF_IMAGE_GRAYSCALE);
	camImage.getPixelsRef().set(0);	
	proImage.getPixelsRef().set(0);
	
	getProCamImages(filename + "/", proImage, camImage, pw, ph, GRAYCODE_MODE_GRAY);
	
	ofDirectory::createDirectory("cam/");
	ofDirectory::createDirectory("pro/");
	camImage.saveImage("cam/" + ofToString(i) + ".png");
	proImage.saveImage("pro/" + ofToString(i) + ".png");
	camImage.update();
	proImage.update();
}

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetDataPathRoot("../../../../../SharedData/");
	
	ofDirectory dir;
	dir.listDir("quick/");
	for(int i = 0; i < dir.size(); i++) {
		decodeAndSave(i, dir.getPath(i));
	}
}

void testApp::update() {
}

void testApp::draw() {
	ofPushMatrix();
	float proImageScale = (ofGetWidth() / 2.) / pw;
	ofScale(proImageScale, proImageScale);
	proImage.draw(0, 0);
	ofPopMatrix();
	
	ofTranslate(ofGetWidth() / 2, 0);
	
	ofPushMatrix();
	float camImageScale = (ofGetWidth() / 2.) / cw;
	ofScale(camImageScale, camImageScale);
	camImage.draw(0, 0);
	ofPopMatrix();
}
