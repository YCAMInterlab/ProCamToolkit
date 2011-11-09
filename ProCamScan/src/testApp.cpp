#include "testApp.h"

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

	FileStorage cfs(ofToDataPath("cam.yml"), FileStorage::READ);
	cfs["cameraMatrix"] >> camMatrix;
	camSize.width = cfs["imageSize_width"];
	camSize.height = cfs["imageSize_height"];
	cfs["distCoeffs"] >> camDistCoeffs;
	
	FileStorage pfs(ofToDataPath("pro.yml"), FileStorage::READ);
	pfs["cameraMatrix"] >> proMatrix;
	proSize.width = pfs["imageSize_width"];
	proSize.height = pfs["imageSize_height"];
	pfs["distCoeffs"] >> proDistCoeffs;
	pfs["rotation"] >> rotation;
	pfs["translation"] >> translation;
	
	camImage.allocate(640, 480, OF_IMAGE_GRAYSCALE);
	proImage.allocate(512, 384, OF_IMAGE_GRAYSCALE);
	
	getRemapPoints("skull/", proImage.getWidth(), proImage.getHeight(), camImagePoints, proImagePoints, colors);
	getProCamImages("skull/", proImage, camImage, proImage.getWidth(), proImage.getHeight());
	camImage.update();
	proImage.update();
	
	camImage.saveImage("camImage.png");
	proImage.saveImage("proImage.png");
	
	//proDistCoeffs = Mat::zeros(proDistCoeffs.size(), proDistCoeffs.type());
	//camDistCoeffs = Mat::zeros(camDistCoeffs.size(), camDistCoeffs.type());
	
	resultPoints = triangulatePositions(
		camImagePoints, camMatrix, camDistCoeffs,
		proImagePoints, proMatrix, proDistCoeffs,
		rotation, translation);
		
	mesh = drawObjectPoints(resultPoints);
	for(int i = 0; i < colors.size(); i++) {
		mesh.addColor(ofColor(colors[i]));
	}
}

void testApp::update() {
}

void testApp::draw() {
	ofBackground(0);
	
	cam.begin();
	ofScale(1, -1, -1);
	
	glEnable(GL_DEPTH_TEST);
	
	ofTranslate(0, 0, -1200);
	
	ofSetColor(magentaPrint);
	drawCamera("camera", camMatrix, camSize, camImage);
	drawCamera("projector", proMatrix, proSize, proImage, rotation, translation);
		
	ofSetColor(255);
	mesh.draw();
	
	cam.end();
}

void testApp::keyPressed(int key) {
}