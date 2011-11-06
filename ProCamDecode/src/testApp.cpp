#include "testApp.h"

void drawChessboardCorners(cv::Size patternSize, const vector<Point2f>& centers) {
	ofMesh lines;
	ofMesh crosses;
	lines.setMode(OF_PRIMITIVE_LINE_STRIP);
	crosses.setMode(OF_PRIMITIVE_LINES);
	int i = 0;
	float radius = 4;
	ofVec2f firstCross = ofVec2f(0, radius).getRotated(-45);
	ofVec2f secondCross = ofVec2f(0, radius).getRotated(+45);
	ofNoFill();
	for(int y = 0; y < patternSize.height; y++) {
		for(int x = 0; x < patternSize.width; x++) {
			if(i == centers.size()) {
				return;
			}
			ofVec2f cur = toOf(centers[i++]);
			ofCircle(cur, radius);
			crosses.addVertex(cur - firstCross);
			crosses.addVertex(cur + firstCross);
			crosses.addVertex(cur - secondCross);
			crosses.addVertex(cur + secondCross);
			lines.addVertex(cur);
		}
	}
	lines.draw();
	crosses.draw();
}

vector<cv::Point2f> getCenters(ofImage& img, cv::Size patternSize) {
	Mat imgMat = toCv(img);
	vector<cv::Point2f> centers;
	bool found = findCirclesGrid(imgMat, patternSize, centers, CALIB_CB_CLUSTERING | CALIB_CB_ASYMMETRIC_GRID);
	if(!found) {
		cout << "failed to find circles" << endl;
	}
	return centers;
}

void testApp::decodeAndSave(string filename) {
	cout << "loading " << filename << endl;

	camImage.allocate(640, 480, OF_IMAGE_GRAYSCALE);
	camImage.getPixelsRef().set(0);
	
	proImage.allocate(1024/2, 768/2, OF_IMAGE_GRAYSCALE);
	proImage.getPixelsRef().set(0);
	
	patternSize = cv::Size(4, 11);
	
	getProCamImages(filename + "/", proImage, camImage, 1024/2, 768/2);
	
	camImage.saveImage("results/" + filename + "-camImage.png");
	camImage.update();
	
	proImage.saveImage("results/" + filename + "-proImage.png");
	proImage.update();
	
	camCenters = getCenters(camImage, patternSize);
	proCenters = getCenters(proImage, patternSize);
}

void testApp::setup() {
	ofSetVerticalSync(true);
	ofDirectory calibration;
	calibration.listDir("half-calibration");
	for(int i = 0; i < calibration.size(); i++) {
		decodeAndSave(calibration.getPath(i));
	}
}

void testApp::update() {
}

void testApp::draw() {
	ofScale(.75, .75);
	camImage.draw(0, 0);
	drawChessboardCorners(patternSize, camCenters);
	
	ofTranslate(0, 480);
	proImage.draw(0, 0);
	drawChessboardCorners(patternSize, proCenters);
}
