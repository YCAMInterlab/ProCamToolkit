#pragma once

#include "ofMain.h"
#include "ofxCv.h"

enum GrayCodeMode {GRAYCODE_MODE_OPPOSITES, GRAYCODE_MODE_GRAY};

// todo: add getRemapPoints + getProCamImages in one pass
void getRemapPoints(string filename, int width, int height, vector<cv::Point2f>& camImagePoints, vector<cv::Point2f>& proImagePoints, vector<unsigned char>& colors, GrayCodeMode mode);
void getProCamImages(string path, cv::Mat& pro, cv::Mat& cam, int width, int height, GrayCodeMode mode);
void grayDecode(string path, cv::Mat& binaryCoded, cv::Mat& cam, GrayCodeMode mode);

void thresholdedToBinary(vector<cv::Mat>& thresholded, cv::Mat& binaryCoded);
void grayToBinary(cv::Mat& binaryCoded, int n);

cv::Mat buildRemap(cv::Mat& binaryCodedX, cv::Mat& binaryCodedY, cv::Mat& mask, int tw, int th);
void applyRemap(cv::Mat& remap, cv::Mat& input, cv::Mat& output, int width, int height);

// you have to handle allocation
template <class C, class P>
void getProCamImages(string path, C& cam, P& pro, int width, int height, GrayCodeMode mode) {
	cv::Mat camMat = toCv(cam);
	cv::Mat proMat = toCv(pro);
	getProCamImages(path, camMat, proMat, width, height, mode);
}

void drawChessboardCorners(cv::Size patternSize, const vector<cv::Point2f>& centers);
vector<vector<cv::Point3f> > buildObjectPoints(cv::Size patternSize, float squareSize, int objectCount, ofxCv::CalibrationPattern patternType);

vector<cv::Point3f> triangulatePositions(
	vector<cv::Point2f>& camImagePoints, cv::Mat camMatrix, cv::Mat camDistCoeffs,
	vector<cv::Point2f>& proImagePoints, cv::Mat proMatrix, cv::Mat proDistCoeffs,
																		 cv::Mat proRotation, cv::Mat proTranslation);
void drawCamera(cv::Mat camMatrix, cv::Size size, float scale, ofImage& img);
void drawLabeledAxes(float size);
void drawImagePoints(cv::Mat camMatrix, vector<cv::Point2f>& imagePoints, float scale);
ofMesh drawObjectPoints(vector<cv::Point3f>& points);
void drawObjectPoints(vector<cv::Point3f>& points, cv::Mat rotation, cv::Mat translation);
void drawCamera(string name, cv::Mat camMatrix, cv::Size size,
	ofImage& image,
	cv::Mat rotation = cv::Mat(), cv::Mat translation = cv::Mat());
void drawCamera(string name, cv::Mat camMatrix, cv::Size size,
	vector<cv::Point3f>& objectPoints, cv::Mat objectRotation, cv::Mat objectTranslation,
	vector<cv::Point2f>& imagePoints,
	ofImage& image,
	cv::Mat rotation = cv::Mat(), cv::Mat translation = cv::Mat());

ofVec3f ofWorldToScreen(ofVec3f world);
ofVec3f ofScreenToWorld(ofVec3f screen);
ofMesh getProjectedMesh(const ofMesh& mesh);
cv::Point2f getClosestPoint(const vector<cv::Point2f>& vertices, float x, float y, int* choice = NULL, float* distance = NULL);
ofVec3f getClosestPointOnMesh(const ofMesh& mesh, float x, float y, int* choice = NULL, float* distance = NULL);

void exportPlyCloud(string filename, ofMesh& cloud);