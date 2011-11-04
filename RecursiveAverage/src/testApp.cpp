#include "testApp.h"

using namespace ofxCv;
using namespace cv;


int recursiveSum(string path, Mat& sum) {
	int total = 0;
	ofDirectory dir;
	dir.listDir(path);
	ofImage img;
	for(int i = 0; i < dir.size(); i++) {
		string cur = dir.getPath(i);
		if(dir[i].isDirectory()) {
			total += recursiveSum(cur, sum);
		} else {
			ofLogVerbose() << "adding " << cur;
			img.loadImage(cur);
			Mat imgMat = toCv(img);
			imitate(sum, img, CV_MAKETYPE(CV_32F, imgMat.channels()));
			accumulate(imgMat, sum);
			total++;
		}
	}
	return total;
}

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	int total = recursiveSum("./", sum);
	sum /= total;
	
	ofPixels average;
	imitate(average, sum, CV_MAKETYPE(CV_8U, sum.channels()));
	Mat averageMat = toCv(average);
	sum.convertTo(averageMat, averageMat.type(), 1., 0.);
	ofSaveImage(average, "average.png");
}

void testApp::update() {
}

void testApp::draw() {
	ofScale(.2, .2);
	drawMat(sum, 0, 0);
}
