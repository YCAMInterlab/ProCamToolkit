#include "testApp.h"

using namespace cv;
using namespace ofxCv;

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
	curPair = 0;	
	patternSize = cv::Size(4, 11);
	squareSize = 16.5; // units in mm
	
	ofDirectory proDir, camDir;
	proDir.listDir("pro");
	camDir.listDir("cam");
	
	int objectCount = 0;
	for(int i = 0; i < camDir.size(); i++) {
		ofImage curCam, curPro;
		curCam.loadImage(camDir.getPath(i));
		curPro.loadImage(proDir.getPath(i));
		Mat camMat = toCv(curCam);
		Mat proMat = toCv(curPro);
		vector<Point2f> curCamImagePoints, curProImagePoints;
		bool foundCam = findCirclesGrid(camMat, patternSize, curCamImagePoints, CALIB_CB_CLUSTERING | CALIB_CB_ASYMMETRIC_GRID);
		bool foundPro = findCirclesGrid(proMat, patternSize, curProImagePoints, CALIB_CB_CLUSTERING | CALIB_CB_ASYMMETRIC_GRID);
		if(foundCam && foundPro) {
			camImagePoints.push_back(curCamImagePoints);
			proImagePoints.push_back(curProImagePoints);
			camImages.push_back(curCam);
			proImages.push_back(curPro);
			objectCount++;
		}
		camSize = camMat.size();
		proSize = proMat.size();
	}
	
	cout << "detecting objects: " << objectCount << endl;
	
	camMatrix = Mat::eye(3, 3, CV_64F);
	camDistCoeffs = Mat::zeros(8, 1, CV_64F);
	proMatrix = Mat::eye(3, 3, CV_64F);
	proDistCoeffs = Mat::zeros(8, 1, CV_64F);
	
	objectPoints = buildObjectPoints(patternSize, squareSize, objectCount, ASYMMETRIC_CIRCLES_GRID);
	int calibFlags = 0;
	float camRms = calibrateCamera(objectPoints, camImagePoints, camSize, camMatrix, camDistCoeffs, camRot, camTrans, calibFlags);
	float proRms = calibrateCamera(objectPoints, proImagePoints, proSize, proMatrix, proDistCoeffs, proRot, proTrans, calibFlags);

	cout << "cam has " << camRms << endl;
	cout << "cam matrix " << camMatrix << endl;
	cout << "pro has " << proRms << endl;
	cout << "pro matrix " << proMatrix << endl;
	
	// uses CALIB_FIX_INTRINSIC by default
	stereoCalibrate(objectPoints,
									proImagePoints, camImagePoints, // A, B
									proMatrix, proDistCoeffs, // A, A
									camMatrix, camDistCoeffs, // B, B
									camSize, rotation, translation,
									essentialMatrix, fundamentalMatrix);
	// returns the results rotation and translation
	// rotation * A + translation = B
	// e.g., A is projector, B is camera
	// translation : [-332.5760929883801; 255.8786193463019; -117.7930503532803]
	// the projector is to the left, beneath, behind the camera
	
	cout << "translation: " << translation << endl;
	cout << "rotation: " << rotation << endl;
	
	Mat camMatrixInv = camMatrix.inv();
	Mat proMatrixInv = proMatrix.inv();
	
	cout << "cam inv: " << camMatrixInv << endl;
	cout << "pro inv: " << proMatrixInv << endl;
	
	cout << "cam dist: " << camDistCoeffs << endl;
	cout << "pro dist: " << proDistCoeffs << endl;
	
	FileStorage cfs(ofToDataPath("cam.yml"), FileStorage::WRITE);
	cfs << "cameraMatrix" << camMatrix;
	cfs << "imageSize_width" << camSize.width;
	cfs << "imageSize_height" << camSize.height;
	cfs << "distCoeffs" << camDistCoeffs;
	cfs << "reprojectionError" << camRms;
	
	FileStorage pfs(ofToDataPath("pro.yml"), FileStorage::WRITE);
	pfs << "cameraMatrix" << proMatrix;
	pfs << "imageSize_width" << proSize.width;
	pfs << "imageSize_height" << proSize.height;
	pfs << "distCoeffs" << proDistCoeffs;
	pfs << "reprojectionError" << proRms;
	pfs << "rotation" << rotation;
	pfs << "translation" << translation;
	
	resultPoints.resize(objectCount);
	for(int i = 0; i < objectCount; i++) {
		resultPoints[i] = triangulatePositions(
			camImagePoints[i], camMatrix, camDistCoeffs,
			proImagePoints[i], proMatrix, proDistCoeffs,
			rotation, translation);
	}
}

void testApp::update() {
}

void testApp::draw() {
/*
	camImages[curPair].draw(0, 0);
	drawChessboardCorners(patternSize, camImagePoints[curPair]);
	
	ofTranslate(640, 0);
	proImages[curPair].draw(0, 0);
	drawChessboardCorners(patternSize, proImagePoints[curPair]);
	*/
	
	ofBackground(0);
	
	cam.begin();
	ofScale(1, -1, -1);
	
	glEnable(GL_DEPTH_TEST);
	
	ofTranslate(0, 0, -1200);
	
	ofSetColor(magentaPrint);
	drawCamera("camera",
		camMatrix, camSize,
		objectPoints[curPair], camRot[curPair], camTrans[curPair],
		camImagePoints[curPair],
		camImages[curPair]);
		
	ofSetColor(cyanPrint);
	drawCamera("projector",
		proMatrix, proSize,
		objectPoints[curPair], proRot[curPair], proTrans[curPair],
		proImagePoints[curPair],
		proImages[curPair],
		rotation, translation);
		
	ofSetColor(yellowPrint);
	drawObjectPoints(resultPoints[curPair]);
	
	cam.end();
}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_UP) {
		curPair--;
	}
	if(key == OF_KEY_DOWN) {
		curPair++;
	}
	curPair = ofClamp(curPair, 0, camImages.size() - 1);
}