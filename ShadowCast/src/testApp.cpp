#include "testApp.h"

const float fovWidth = 1.0144686707507438;
const float fovHeight = 0.78980943449644714;
const float XtoZ = tanf(fovWidth/2)*2;
const float YtoZ = tanf(fovHeight/2)*2;
const unsigned int camWidth = 640;
const unsigned int camHeight = 480;

ofVec3f ConvertProjectiveToRealWorld(ofVec3f projective) {
	return ofVec3f((projective.x/camWidth-.5f) * projective.z * XtoZ,
								 (projective.y/camHeight-.5f) * projective.z * YtoZ,
								 projective.z);
}

using namespace ofxCv;
using namespace cv;

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetDataPathRoot("../../../../../SharedData/");
	
	panel.setup();
	panel.addPanel("Control Panel");
	panel.addSlider("sceneOffsetX", .28, -1, 1);
	panel.addSlider("sceneOffsetY", .18, -1, 1);
	panel.addSlider("sceneScale", .43, 0, 2);
	panel.addSlider("shojiBlur", 1, 0, 2);
	
	panel.addPanel("Presence");
	panel.addToggle("automaticFading", false);
	panel.addSlider("fadeInTime", .5, 0, 1);
	panel.addSlider("fadeOutTime", .5, 0, 1);
	panel.addSlider("fadeOutDelay", .5, 0, 1);
	panel.addSlider("presenceRate", .5, 0, 1);
	panel.addSlider("presence", 1, 0, 2); //
	panel.addToggle("fading", false); //
	panel.addSlider("fade", 0, 0, 1); //
	
	panel.addPanel("Detection");
	panel.addSlider("kinectNear", 500, 0, 2000);
	panel.addSlider("kinectFar", 1500, 0, 2000);
	panel.addSlider("kinectBlur", 16, 0, 32, true);
	panel.addSlider("threshold", 200, 0, 256);
	panel.addSlider("minAreaRadius", 30, 0, 100);
	panel.addSlider("maxAreaRadius", 200, 0, 200);
	panel.addSlider("minx", -300, -1000, 1000);
	panel.addSlider("maxx", +100, -1000, 1000);
	panel.addSlider("miny", -100, -1000, 1000);
	panel.addSlider("maxy", +100, -1000, 1000	);
	panel.addSlider("touchOffset", -.4 * sceneHeight, -sceneHeight, sceneHeight);
	
	sceneFbo.allocate(sceneWidth, sceneHeight);
	shojiBlur.setup(sceneWidth, sceneHeight, 8);
	
	leftRemap.setup("remap-left.exr", "left-lut.csv");
	rightRemap.setup("remap-right.exr", "right-lut.csv");
	
	leftRemap.setScene(sceneFbo.getTextureReference());
	rightRemap.setScene(sceneFbo.getTextureReference());
	leftRemap.setShoji(shojiBlur.getTextureReference(), +1);
	rightRemap.setShoji(shojiBlur.getTextureReference(), -1);
	
	showDebug = false;
	whiteScene = true;
	
	scenes.push_back(new OrigScene());
	scenes.push_back(new GridScene());
	scenes.push_back(new LensScene());
	scenes.push_back(new BallScene());
	scenes.push_back(new VoroScene());
	scenes.push_back(new WindScene());
	scenes.push_back(new RectScene());
	scenes.push_back(new WallScene());
	scenes.push_back(new DotsScene());
	scenes.push_back(new FracScene());
	scenes.push_back(new BoidScene());
	for(int i = 0; i < scenes.size(); i++) {
		scenes[i]->setSize(sceneWidth, sceneHeight);
		scenes[i]->setup();
	}
	curScene = 0;
	
	kinect.setDepthClipping(1000, 5000);
	kinect.init(false, false);
	kinect.open();
	
	presence = 1;
	lastPresence = 1;
	startFadeTime = 0;
	fadeState = 0;
}

void testApp::drawScene() {
	if(whiteScene) {
		ofClear(255, 255);
	} else {
		ofClear(0, 255);
		ofTranslate(sceneWidth / 2, sceneHeight / 2);
		ofRotate(ofGetElapsedTimef());
		ofSetLineWidth(4);
		ofSetColor(255);
		ofMesh mesh;
		mesh.setMode(OF_PRIMITIVE_LINES);
		for(int y = -sceneHeight; y < sceneHeight; y += 64) {
			for(int x = -sceneWidth; x < sceneWidth; x += 64) {
				mesh.addVertex(ofVec2f(-sceneWidth, y));
				mesh.addVertex(ofVec2f(+sceneWidth, y));
				mesh.addVertex(ofVec2f(x, -sceneHeight));
				mesh.addVertex(ofVec2f(x, +sceneHeight));
			}
		}
		mesh.draw();
	}
}

void testApp::drawShoji() {
	scenes[curScene]->draw();
	
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(ofFloatColor(0, panel.getValueF("fade")));
	ofRect(0, 0, sceneWidth, sceneHeight);
	ofDisableAlphaBlending();	
	ofPopStyle();
}

void testApp::drawOverlay() {
	ofPushMatrix();
	ofScale(.5, .5);
	kinect.drawDepth(0, 0);
	contourFinder.draw();
	for(int i = 0; i < contourFinder.size(); i++) {
		ofVec2f curCenter = toOf(contourFinder.getCenter(i));
		ofCircle(curCenter, 10);
		ofVec3f cur = users[i];
		ofVec3f curBefore = usersBefore[i];
		drawHighlightString(ofToString(userLabels[i]) + ": " + 
			ofToString((int) curBefore.x) + "/" + ofToString((int) curBefore.y) + "->" +
			ofToString((int) cur.x) + "/" + ofToString((int) cur.y),
			curCenter);
	}
	ofPopMatrix();
}

void testApp::pickNextScene() {
	curScene = (curScene + 1) % scenes.size();
}

void testApp::setFade() {
	if(panel.getValueB("automaticFading")) {
		float curTime = ofGetElapsedTimef();
		bool curPresent = presence < 1.5;
		bool lastPresent = lastPresence < 1.5;
		if(curPresent != lastPresent) {
			if(curPresent) {
				pickNextScene();
			}
			startFadeTime = curTime;
		}
		
		if(curPresent) {
			fadeState = 1 - ((curTime - startFadeTime) / panel.getValueF("fadeInTime"));
		} else {
			fadeState = (curTime - (startFadeTime + panel.getValueF("fadeOutDelay"))) / panel.getValueF("fadeOutTime");
		}
		
		bool fading = fadeState > 0 && fadeState < 1;
		panel.setValueB("fading", fading);
		fadeState = ofClamp(fadeState, 0, 1);
		panel.setValueF("fade", fadeState);
		
		lastPresence = presence;
	}
}

void testApp::findUserLocations() {	
	users.clear();
	usersBefore.clear();
	userLabels.clear();
	int w = kinect.getWidth();
	float* distancePixels = kinect.getDistancePixels();
	ofVec2f centerNorm(.5, .5);
	float maxDistance = centerNorm.distance(ofVec2f(0));
	float minDistance = 2 * maxDistance;
	imitate(filled, kinect, CV_8UC1);
	for(int i = 0; i < contourFinder.size(); i++) {
		fillPoly(contourFinder.getContour(i), filled);
		cv::Rect roi = contourFinder.getBoundingRect(i);
		cv::Point tl = roi.tl(), br = roi.br();
		ofVec3f sum;
		unsigned char* filledPixels = filled.getPixels();
		int count = 0;
		for(int y = tl.y; y < br.y; y++) {
			for(int x = tl.x; x < br.x; x++) {
				int i = y * w + x;
				if(filledPixels[i] > 128) {
					sum.x += x;
					sum.y += y;
					sum.z += distancePixels[i];
					count++;
				}
			}
		}
		ofVec3f cur = ConvertProjectiveToRealWorld(sum / count);
		usersBefore.push_back(cur);
		cur.x = ofMap(cur.x, panel.getValueF("minx"), panel.getValueF("maxx"), 0, 1);
		cur.y = ofMap(cur.y, panel.getValueF("miny"), panel.getValueF("maxy"), 0, 1);
		ofVec2f curNorm(cur.x, cur.y);
		cur.x *= sceneWidth;
		cur.y *= sceneHeight;
		users.push_back(cur);
		userLabels.push_back(contourFinder.getLabel(i));
		float distance = centerNorm.distance(curNorm);
		if(i == 0 || distance < minDistance) {
			minDistance = distance;
		}
	}
	float curPresence = minDistance / maxDistance;
	presence = ofLerp(presence, curPresence, panel.getValueF("presenceRate"));
	panel.setValueF("presence", presence);
}

void testApp::updateScene() {
	vector<ofVec2f> touches;
	ofSeedRandom(0); // ofPushSeedRandom()
	int noisePoints = 0;
	for(int i = 0; i < noisePoints; i++) {
		float f = .1;
		touches.push_back(
			ofVec2f(
				sceneWidth * ofNoise(ofGetElapsedTimef() * f + ofRandom(1024)),
				sceneHeight * ofNoise(ofGetElapsedTimef() * f + ofRandom(1024))));
	}
	ofSeedRandom(); // ofPopSeedRandom()
	for(int i = 0; i < users.size(); i++) {
		touches.push_back(users[i] + ofVec2f(0, panel.getValueF("touchOffset")));
	}
	if(ofGetMousePressed()) {
		touches.push_back(ofVec2f(mouseX, mouseY));
	}
	scenes[curScene]->setTouches(touches);
	scenes[curScene]->update();
}
	
void testApp::update(){
	if(panel.hasValueChanged("kinectFar") || panel.hasValueChanged("kinectNear")) {
		kinect.setDepthClipping(panel.getValueF("kinectNear"), panel.getValueF("kinectFar"));
	}

	kinect.update();
	if(kinect.isFrameNew()) {		
		contourFinder.setInvert(false);
		contourFinder.setThreshold(panel.getValueF("threshold"));
		contourFinder.setMinAreaRadius(panel.getValueF("minAreaRadius"));
		contourFinder.setMaxAreaRadius(panel.getValueF("maxAreaRadius"));
		Mat depthMat = Mat(kinect.getHeight(), kinect.getWidth(), CV_8UC1, kinect.getDepthPixels(), 0);
		if(kinectAccumulator.empty()) {
			depthMat.convertTo(kinectAccumulator, CV_32F);
		}
		blur(kinectAccumulator, panel.getValueF("kinectBlur"));
		kinectAccumulator.convertTo(kinectAccumulator8u, CV_8U);
		accumulateWeighted(depthMat, kinectAccumulator, .1);
		contourFinder.findContours(kinectAccumulator8u);
		findUserLocations();
		setFade();
	}

	float sceneScale = panel.getValueF("sceneScale");
	ofVec2f sceneOffset(ofVec2f(panel.getValueF("sceneOffsetX"), panel.getValueF("sceneOffsetY")));
	leftRemap.setScale(sceneScale);
	leftRemap.setOffset(sceneOffset);
	rightRemap.setScale(sceneScale);
	rightRemap.setOffset(sceneOffset);
	
	updateScene();
	
	panel.clearAllChanged();
}

void testApp::draw() {
	ofBackground(0);
	
	ofSetColor(255);
	
	// render scene
	sceneFbo.begin();
	drawScene();
	sceneFbo.end();
	
	// render and blur shoji
	shojiBlur.setScale(panel.getValueF("shojiBlur"));
	shojiBlur.begin();
	drawShoji();
	shojiBlur.end();

	// warp scene
	leftRemap.update();
	rightRemap.update();
	
	// draw scene to screen
	ofPushMatrix();
	float scaleAmount = (float) ofGetHeight() / th;
	ofScale(scaleAmount, scaleAmount);
	
	ofBackground(0);
	if(showDebug) {
		sceneFbo.getTextureReference().draw(0, 0);
		shojiBlur.getTextureReference().draw(tw, 0);
	} else {
		leftRemap.getTextureReference().draw(0, 0);
		rightRemap.getTextureReference().draw(tw, 0);
	}
	ofPopMatrix();
	
	// draw overlay
	if(!panel.hidden) {
		drawOverlay();
	}
}

void testApp::keyPressed(int key) {
	if(key == 'f') {
		//ofToggleFullscreen();
	}
	if(key == 'd') {
		showDebug = !showDebug;
	}
	if(key == 'w') {
		whiteScene = !whiteScene;
	}
	if(key == 'r') {
		scenes[curScene]->reload(); 
	}
	
	if(key == OF_KEY_UP) {
		curScene--;
	}
	if(key == OF_KEY_DOWN) {
		curScene++;
	}
	curScene = ofClamp(curScene, 0, scenes.size() - 1);
	//cout << curScene << endl;
}