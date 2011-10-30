#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setb(string name, bool value) {
	panel.setValueB(name, value);
}
void testApp::seti(string name, int value) {
	panel.setValueI(name, value);
}
void testApp::setf(string name, float value) {
	panel.setValueF(name, value);
}
bool testApp::getb(string name) {
	return panel.getValueB(name);
}
int testApp::geti(string name) {
	return panel.getValueI(name);
}
float testApp::getf(string name) {
	return panel.getValueF(name);
}

void testApp::setup() {
	ofSetVerticalSync(true);
	calibrationReady = false;
	setupMesh();	
	setupControlPanel();
}

void testApp::update() {
	if(getb("randomLighting")) {
		setf("lightX", ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000);
		setf("lightY", ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000);
		setf("lightZ", ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000);
	}
	light.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));
		
	if(getb("selectionMode")) {
	} else {
		updateRenderMode();
	}
}

void enableFog(float near, float far) {
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4]= {0, 0, 0, 1};
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogf(GL_FOG_START, near);
	glFogf(GL_FOG_END, far);
}

void disableFog() {
	glDisable(GL_FOG);
}

void testApp::draw() {
	ofBackground(geti("backgroundColor"));
	glEnable(GL_DEPTH_TEST);
	
	if(getb("selectionMode")) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}
	
	glDisable(GL_DEPTH_TEST);
}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_BACKSPACE) { // delete selected
		if(getb("selected")) {
			setb("selected", false);
			int choice = geti("selectionChoice");
			referencePoints[choice] = false;
			imagePoints[choice] = Point2f();
		}
	}
	if(key == '\n') { // deselect
		setb("selected", false);
	}
	if(key == ' ') { // toggle render/select mode
		setb("selectionMode", !getb("selectionMode"));
	}
	if(key == 's') { // save calibration data
		saveData();
	}
}

void testApp::mousePressed(int x, int y, int button) {
	setb("selected", getb("hoverSelected"));
	seti("selectionChoice", geti("hoverChoice"));
	if(getb("selected")) {
		setb("dragging", true);
	}
}

void testApp::mouseReleased(int x, int y, int button) {
	setb("dragging", false);
}

void testApp::setupMesh() {
	model.loadModel("model.dae");
	objectMesh = model.getMesh(0);
	int n = objectMesh.getNumVertices();
	objectPoints.resize(n);
	imagePoints.resize(n);
	referencePoints.resize(n, false);
	for(int i = 0; i < n; i++) {
		objectPoints[i] = toCv(objectMesh.getVertex(i));
	}
}

void testApp::render() {
	bool useLights = getb("useLights");
	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
	}
	
	objectMesh.clearColors();
	if(getb("highlight")) {
		int n = objectMesh.getNumVertices();
		float highlightPosition = getf("highlightPosition");
		float highlightOffset = getf("highlightOffset");
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			objectMesh.addColor(cur);
		}
	}
	
	ofSetColor(255);
	if(getb("drawModel")) {
		if(getb("drawWireframe")) {
			objectMesh.drawWireframe();
		} else {
			objectMesh.drawFaces();
		}
	}
	if(useLights) {
		ofDisableLighting();
	}
}

void testApp::saveData() {
	string dirName = "calibration-" + ofGetTimestampString() + "/";
	ofDirectory dir(dirName);
	dir.create();
	
	FileStorage fs(ofToDataPath(dirName + "intrinsics.yml"), FileStorage::WRITE);
	cv::Size imageSize = intrinsics.getImageSize();
	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;
	fs << "imageSize_width" << imageSize.width;
	fs << "imageSize_height" << imageSize.height;
	
	saveMat(Mat(objectPoints), dirName + "objectPoints.yml");
	saveMat(Mat(imagePoints), dirName + "imagePoints.yml");
}

void testApp::setupControlPanel() {
	panel.setup();
	panel.msg = "tab hides the panel, space toggles render/selection mode.";
	
	panel.addPanel("Interaction");
	panel.addSlider("backgroundColor", 0, 0, 255, true);
	panel.addToggle("selectionMode", true);
	panel.addToggle("drawModel", true);
	panel.addToggle("drawWireframe", true);
	panel.addToggle("useLights", false);
	panel.addToggle("CV_CALIB_FIX_PRINCIPAL_POINT", false);
	
	panel.addPanel("Highlight");
	panel.addToggle("highlight", false);
	panel.addSlider("highlightPosition", 0, 0, 1);
	panel.addSlider("highlightOffset", .1, 0, 1);
	
	panel.addPanel("Calibration");
	panel.addSlider("aov", 80, 50, 100);
	panel.addToggle("CV_CALIB_FIX_ASPECT_RATIO", true);
	panel.addToggle("CV_CALIB_FIX_K1", true);
	panel.addToggle("CV_CALIB_FIX_K2", true);
	panel.addToggle("CV_CALIB_FIX_K3", true);
	panel.addToggle("CV_CALIB_ZERO_TANGENT_DIST", true);
	
	panel.addPanel("Rendering");
	panel.addSlider("fogNear", 200, 0, 1000);
	panel.addSlider("fogFar", 1850, 1000, 2500);
	panel.addSlider("screenPointSize", 6, 1, 16, true);
	panel.addSlider("selectedPointSize", 8, 1, 16, true);
	panel.addSlider("selectionRadius", 12, 1, 32);
	panel.addSlider("lightX", 0, -1000, 1000);
	panel.addSlider("lightY", 0, -1000, 1000);
	panel.addSlider("lightZ", 0, -1000, 1000);
	panel.addToggle("randomLighting", true);
	
	panel.addPanel("Internal");
	panel.addToggle("hoverSelected", false);
	panel.addSlider("hoverChoice", 0, 0, objectPoints.size(), true);
	panel.addToggle("selected", false);
	panel.addToggle("dragging", false);
	panel.addSlider("selectionChoice", 0, 0, objectPoints.size(), true);
	panel.addSlider("slowLerpRate", .001, 0, .01);
	panel.addSlider("fastLerpRate", .05, 0, .1);
}

void testApp::updateRenderMode() {
	// generate camera matrix given aov guess
	float aov = getf("aov");
	Size2i imageSize(ofGetWidth(), ofGetHeight());
	float f = imageSize.width * ofDegToRad(aov); // i think this is wrong, but it's optimized out anyway
	Point2f c = Point2f(imageSize) * (1. / 2);
	Mat1d cameraMatrix = (Mat1d(3, 3) <<
		f, 0, c.x,
		0, f, c.y,
		0, 0, 1);
		
	// generate flags
	#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
	int flags =
		CV_CALIB_USE_INTRINSIC_GUESS |
		getFlag(CV_CALIB_FIX_PRINCIPAL_POINT) |
		getFlag(CV_CALIB_FIX_ASPECT_RATIO) |
		getFlag(CV_CALIB_FIX_K1) |
		getFlag(CV_CALIB_FIX_K2) |
		getFlag(CV_CALIB_FIX_K3) |
		getFlag(CV_CALIB_ZERO_TANGENT_DIST);
	
	vector<Mat> rvecs, tvecs;
	Mat distCoeffs;
	vector<vector<Point3f> > referenceObjectPoints(1);
	vector<vector<Point2f> > referenceImagePoints(1);
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			referenceObjectPoints[0].push_back(objectPoints[i]);
			referenceImagePoints[0].push_back(imagePoints[i]);
		}
	}
	const static int minPoints = 6;
	if(referenceObjectPoints[0].size() >= minPoints) {
		calibrateCamera(referenceObjectPoints, referenceImagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);
		rvec = rvecs[0];
		tvec = tvecs[0];
		intrinsics.setup(cameraMatrix, imageSize);
		modelMatrix = makeMatrix(rvec, tvec);
		calibrationReady = true;
	} else {
		calibrationReady = false;
	}
}

void testApp::drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);
	float w = ofGetWidth();
	float h = ofGetHeight();
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, geti("selectedPointSize"));
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}
	
void testApp::drawSelectionMode() {
	ofSetColor(255);
	cam.begin();
	enableFog(getf("fogNear"), getf("fogFar"));
	render();
	disableFog();
	imageMesh = getProjectedMesh(objectMesh);	
	cam.end();
	
	// draw all reference points cyan
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			drawLabeledPoint(i, imageMesh.getVertex(i), cyanPrint);
		}
	}
	
	// check to see if anything is selected
	// draw hover point magenta
	int choice;
	float distance;
	ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
	if(distance < getf("selectionRadius")) {
		seti("hoverChoice", choice);
		setb("hoverSelected", true);
		drawLabeledPoint(choice, selected, magentaPrint);
	} else {
		setb("hoverSelected", false);
	}
	
	// draw selected point yellow
	if(getb("selected")) {
		int choice = geti("selectionChoice");
		ofVec2f selected = imageMesh.getVertex(choice);
		drawLabeledPoint(choice, selected, yellowPrint, ofColor::white, ofColor::black);
	}
	
	// draw all points cyan small
	glPointSize(geti("screenPointSize"));
	glEnable(GL_POINT_SMOOTH);
	ofSetColor(cyanPrint);
	imageMesh.drawVertices();
}

void testApp::drawRenderMode() {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(calibrationReady) {
		intrinsics.loadProjectionMatrix(10, 2000);
		applyMatrix(modelMatrix);
		render();
		imageMesh = getProjectedMesh(objectMesh);	
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	// draw all reference points cyan
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			drawLabeledPoint(i, toOf(imagePoints[i]), cyanPrint);
		}
	}
	
	// move points that need to be dragged
	// draw selected yellow
	int choice = geti("selectionChoice");
	if(getb("selected")) {
		referencePoints[choice] = true;	
		Point2f& cur = imagePoints[choice];	
		if(cur == Point2f()) {
			if(calibrationReady) {
				cur = toCv(ofVec2f(imageMesh.getVertex(choice)));
			} else {
				cur = Point2f(mouseX, mouseY);
			}
		}
	}
	if(getb("dragging")) {
		Point2f& cur = imagePoints[choice];
		float rate = ofGetMousePressed(0) ? getf("slowLerpRate") : getf("fastLerpRate");
		cur = Point2f(ofLerp(cur.x, mouseX, rate), ofLerp(cur.y, mouseY, rate));
		drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
		ofSetColor(ofColor::black);
		ofRect(toOf(cur), 1, 1);
	} else {
		// check to see if anything is selected
		// draw hover magenta
		float distance;
		ofVec2f selected = toOf(getClosestPoint(imagePoints, mouseX, mouseY, &choice, &distance));
		if(referencePoints[choice] && distance < getf("selectionRadius")) {
			seti("hoverChoice", choice);
			setb("hoverSelected", true);
			drawLabeledPoint(choice, selected, magentaPrint);
		} else {
			setb("hoverSelected", false);
		}
	}
}
