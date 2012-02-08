#include "testApp.h"

void thresholdedToBinary(ofShortImage& binaryCoded, vector<Mat>& thresholded) {
	ofLogVerbose() << "thresholded to binary";
	
	imitate(binaryCoded, thresholded[0]);
	unsigned short* binaryCodedPixels = binaryCoded.getPixels();
	int w = binaryCoded.getWidth();
	int h = binaryCoded.getHeight();
	int m = w * h;
	int n = thresholded.size();
	for(int i = 0; i < n; i++) {
		unsigned char* curThreshold = thresholded[i].ptr<unsigned char>();
		for(int k = 0; k < m; k++) {
			if(curThreshold[k]) {
				binaryCodedPixels[k] |= (1 << (n - i - 1));
			}
		}
	}
}

void grayToBinary(ofShortImage& binaryCoded, int n) {
	unsigned short* binaryCodedPixels = binaryCoded.getPixels();
	
	ofLogVerbose() << "gray to binary LUT";
	
	// built gray code to binary LUT
	int codes = 1 << n;
	vector<unsigned short> binaryLUT(codes);
	for(unsigned short binary = 0; binary < codes; binary++) {
		unsigned short gray = (binary >> 1) ^ binary;
		binaryLUT[gray] = binary;
	}
	
	ofLogVerbose() << "gray to binary decode";
	
	// convert gray code to binary using LUT
	int w = binaryCoded.getWidth();
	int h = binaryCoded.getHeight();
	int m = w * h;
	for(int i = 0; i < m; i++) {
		binaryCodedPixels[i] = binaryLUT[binaryCodedPixels[i]];
	}
}

void grayDecode(string path, ofShortImage& binaryCoded) {
	ofDirectory dirNormal, dirInverse;
	dirNormal.listDir(path + "normal/");
	dirInverse.listDir(path + "inverse/");
	
	ofLogVerbose() << "loading, thresholding, min, and max";
	
	// per-pixel threshold of each image
	int n = dirNormal.size();
	vector<Mat> thresholded(n);
	Mat minMat, maxMat;
	for(int i = 0; i < n; i++) {
		ofImage imageNormal, imageInverse;
		imageNormal.loadImage(dirNormal.getPath(i));
		imageNormal.setImageType(OF_IMAGE_GRAYSCALE);
		imageInverse.loadImage(dirInverse.getPath(i));
		imageInverse.setImageType(OF_IMAGE_GRAYSCALE);
		
		Mat curNormal = toCv(imageNormal);
		Mat curInverse = toCv(imageInverse);
		thresholded[i] = curNormal > curInverse;
		
		if(i == 0) {
			minMat = min(curNormal, curInverse);
			maxMat = max(curNormal, curInverse);
		} else {
			min(minMat, curNormal, minMat);
			min(minMat, curInverse, minMat);
			max(maxMat, curNormal, maxMat);
			max(maxMat, curInverse, maxMat);
		}
	}
	
	ofLogVerbose() << "masking";
	
	// masking the threshold is slower but easier to write
	Mat maskMat = (maxMat - minMat) > 30;
	for(int i = 0; i < n; i++) {
		thresholded[i] = thresholded[i].mul(maskMat);
	}
	
	thresholdedToBinary(binaryCoded, thresholded);	
	grayToBinary(binaryCoded, n);
}

// naive nearest neighbor remap
void remap(vector<ofVec2f>& location, ofImage& input, ofImage& output) {
	int w = output.getWidth();
	int h = output.getHeight();
	int iw = input.getWidth();
	int ih = input.getHeight();
	int i = 0;
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			ofVec2f cur = location[i];
			output.setColor(x, y, input.getColor(cur.x, cur.y));
			i++;
		}
	}
}

void buildBinaryCoded(ofShortImage& binaryCodedX, ofShortImage& binaryCodedY, ofImage& binaryCoded) {
	int w = binaryCodedX.getWidth();
	int h = binaryCodedX.getHeight();
	unsigned short* xPixels = binaryCodedX.getPixels();
	unsigned short* yPixels = binaryCodedY.getPixels();
	binaryCoded.allocate(w, h, OF_IMAGE_COLOR);
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			int i = y * w + x;
			if(xPixels[i] != 0 && yPixels[i] != 0) {
				bool curx = xPixels[i] % 2 == 0;
				bool cury = yPixels[i] % 2 == 0;
				binaryCoded.setColor(x, y,
														 cury ?
														 (curx ? ofColor::cyan : ofColor::magenta) :
														 (curx ? ofColor::yellow : ofColor::black));
			} else {
				binaryCoded.setColor(x, y, ofColor::black);
			}
		}
	}
	binaryCoded.update();
}

ofShortImage* curBinaryCodedX;
ofShortImage* curBinaryCodedY;
ofVec2f cameraToProjector(ofVec2f cameraPosition) {
	// right now this isn't interpolated, but it should be
	// especially when the images are smaller
	int w = curBinaryCodedX->getWidth();
	int h = curBinaryCodedY->getHeight();
	cameraPosition.x = ofClamp(cameraPosition.x * w, 0, w - 1);
	cameraPosition.y = ofClamp(cameraPosition.y * h, 0, h - 1);
	int i = ((int) cameraPosition.y) * w + (int) cameraPosition.x;
	unsigned short* xPixels = curBinaryCodedX->getPixels();
	unsigned short* yPixels = curBinaryCodedY->getPixels();
	unsigned short tx = xPixels[i];
	unsigned short ty = yPixels[i];
	ofVec2f projectorPosition(tx, ty);
	return projectorPosition;
}

void saveRemap(ofShortImage& binaryCodedX, ofShortImage& binaryCodedY, string filename) {
	int w = binaryCodedX.getWidth();
	int h = binaryCodedX.getHeight();
	int n = w * h;
	vector<ofVec2f> location(tn);
	vector<int> count(tn);
	unsigned short* xPixels = binaryCodedX.getPixels();
	unsigned short* yPixels = binaryCodedY.getPixels();
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			int i = y * w + x;
			unsigned short tx = xPixels[i];
			unsigned short ty = yPixels[i];
			int ti = ofClamp(ty * tw + tx, 0, tn - 1);
			location[ti] += ofVec2f(x, y);
			count[ti]++;
		}
	}
	
	// normalize by count, then scale locations into input image
	for(int i = 0 ; i < tn; i++) {
		location[i] /= count[i];
		//location[i].x = ofClamp(location[i].x * tw / w, 0, tw - 1);
		//location[i].y = ofClamp(location[i].y * th / h, 0, th - 1);
	}
	
	ofFloatImage remap;
	remap.allocate(tw, th, OF_IMAGE_COLOR);
	float* remapPixels = remap.getPixels();
	for(int i = 0 ; i < tn; i++) {
		remapPixels[i * 3 + 0] = location[i].x / w;
		remapPixels[i * 3 + 1] = location[i].y / h;
	}
	remap.saveImage(filename);
	
	/*
	points.clear();
	points.setMode(OF_PRIMITIVE_POINTS);
	for(int i = 0; i < tn; i++) {
		if(location[i] != ofVec2f()) {
			if((i / tw) % 2 == 0) {
				points.addColor(i % 2 == 0 ? ofColor::cyan : ofColor::magenta);
			} else {
				points.addColor(i % 2 == 0 ? ofColor::yellow : ofColor::white);
			}
			points.addVertex(location[i]);
		}
	}*/
}

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetDataPathRoot("../../../../../SharedData/");
	/*
	panel.setup();
	panel.addPanel("Global");
	panel.addSlider("blurScale", 1., 0, 4, false);
	*/
	leftFbo.allocate(tw, th, GL_RGBA, 2);
	rightFbo.allocate(tw, th, GL_RGBA, 2);
	
	string base = "interlab/";
	if(false || !(binaryCodedLeftX.loadImage(base + "left/vertical.png") &&
								binaryCodedLeftY.loadImage(base + "left/horizontal.png") &&
								binaryCodedRightX.loadImage(base + "right/vertical.png") &&
								binaryCodedRightY.loadImage(base + "right/horizontal.png"))) {
		grayDecode(base + "left/vertical/", binaryCodedLeftX);
		grayDecode(base + "left/horizontal/", binaryCodedLeftY);
		grayDecode(base + "right/vertical/", binaryCodedRightX);
		grayDecode(base + "right/horizontal/", binaryCodedRightY);
		binaryCodedLeftX.saveImage(base + "left/vertical.png");
		binaryCodedLeftY.saveImage(base + "left/horizontal.png");	
		binaryCodedRightX.saveImage(base + "right/vertical.png");
		binaryCodedRightY.saveImage(base + "right/horizontal.png");
	}
	
	saveRemap(binaryCodedLeftX, binaryCodedLeftY, "remap-left.exr");
	saveRemap(binaryCodedRightX, binaryCodedRightY, "remap-right.exr");
	
	//leftLut.setup(tw, th, "left-lut.exr");
	//rightLut.setup(tw, th, "right-lut.exr");
	
	leftLut.setupText(tw, th, "left-lut.csv");
	rightLut.setupText(tw, th, "right-lut.csv");
	
	//buildBinaryCoded(binaryCodedLeftX, binaryCodedLeftY, binaryCodedLeft);
	//buildBinaryCoded(binaryCodedRightX, binaryCodedRightY, binaryCodedRight);
}

void testApp::update() {
}

bool _inverse = false;
void setInverse(bool inverse) {
	_inverse = inverse;
}
ofColor full(const ofColor& cur) {
	ofColor actual;
	if(_inverse) {
		actual = ofColor::white - cur;
	} else {
		actual = cur;
	}
	return actual;
}

void addQuad(ofMesh& quads, ofColor color, ofVec2f topLeft, ofVec2f size) {
	ofColor cur = full(color);
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, 0)));
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, size.y)));
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, 0)));
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, 0)));
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, size.y)));
	quads.addColor(cur);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, size.y)));
}

void addQuadGradient(ofMesh& quads, ofColor start, ofColor end, ofVec2f topLeft, ofVec2f size) {
	ofColor curStart = full(start);
	ofColor curEnd = full(end);
	quads.addColor(curStart);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, 0)));
	quads.addColor(curEnd);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, size.y)));
	quads.addColor(curStart);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, 0)));
	quads.addColor(curStart);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, 0)));
	quads.addColor(curEnd);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(0, size.y)));
	quads.addColor(curEnd);
	quads.addVertex(cameraToProjector(topLeft + ofVec2f(size.x, size.y)));
}

void testApp::drawProjector() {
	float sx = .03;
	float sy = .4;
	float b = .03;
	
	ofVec2f cur((float) mouseX / ofGetWidth(), (float) mouseY / ofGetHeight());
	if(!ofGetMousePressed()) {
		float radius = .04;
		float theta = ofGetElapsedTimef();
		cur.x = .5 + (cos(theta) * radius);
		cur.y = .5 + (sin(theta) * radius);
	}
	cur.x -= (b + 4*(b + sx)) / 2;
	cur.y -= (b + (b+sy)) / 2;
	ofFill();
	
	ofMesh quads;
	quads.setMode(OF_PRIMITIVE_TRIANGLES);
	addQuad(quads, ofColor::white, cur, ofVec2f(b + 4*(b + sx), b + (b+sy)));
	addQuadGradient(quads, ofColor::black, ofColor::white, cur + ofVec2f(b + 0*(b + sx),b), ofVec2f(sx, sy));
	addQuadGradient(quads, ofColor::black, ofColor::white, cur + ofVec2f(b + 1*(b + sx),b), ofVec2f(sx, sy));
	addQuadGradient(quads, ofColor::black, ofColor::white, cur + ofVec2f(b + 2*(b + sx),b), ofVec2f(sx, sy));
	addQuadGradient(quads, ofColor::black, ofColor::white, cur + ofVec2f(b + 3*(b + sx),b), ofVec2f(sx, sy));
	/*
	 addQuadGradient(quads, ofColor::black, ofColor::white, cur + ofVec2f(b + 0*(b + sx),b), ofVec2f(sx, sy));
	 addQuadGradient(quads, ofColor::black, ofColor::red, cur + ofVec2f(b + 1*(b + sx),b), ofVec2f(sx, sy));
	 addQuadGradient(quads, ofColor::black, ofColor::green, cur + ofVec2f(b + 2*(b + sx),b), ofVec2f(sx, sy));
	 addQuadGradient(quads, ofColor::black, ofColor::blue, cur + ofVec2f(b + 3*(b + sx),b), ofVec2f(sx, sy));
	 */
	quads.drawFaces();
}

void testApp::draw() {
	ofPushMatrix();
	if(ofGetHeight() != th) {
		float scaleAmount = (float) ofGetHeight() / th;
		ofScale(scaleAmount, scaleAmount);
	}
	
	ofBackground(0);
	ofSetColor(255);
	
	setInverse(false);
	curBinaryCodedX = &binaryCodedLeftX;
	curBinaryCodedY = &binaryCodedLeftY;
	leftFbo.begin();
	ofClear(0, 255);
	ofPushMatrix();
	drawProjector();
	ofPopMatrix();
	leftFbo.end();
	
	leftLut.setSource(leftFbo.getTextureReference());
	leftLut.update();
	leftLut.getTextureReference().draw(0, 0);
	
	setInverse(true);
	curBinaryCodedX = &binaryCodedRightX;
	curBinaryCodedY = &binaryCodedRightY;
	rightFbo.begin();
	ofClear(0, 255);
	ofPushMatrix();
	drawProjector();
	ofPopMatrix();
	rightFbo.end();
	
	rightLut.setSource(rightFbo.getTextureReference());
	rightLut.update();
	rightLut.getTextureReference().draw(tw, 0);
	
	ofPopMatrix();
}

void testApp::keyPressed(int key) {
	if(key == 'f') {
		//ofToggleFullscreen();
	}
}