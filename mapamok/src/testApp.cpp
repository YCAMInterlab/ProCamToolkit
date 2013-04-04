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
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
	calibrationReady = false;
	setupMesh();	
	setupControlPanel();
}

void testApp::update() {
	ofSetWindowTitle("mapamok");
	if(getb("randomLighting")) {
		setf("lightX", ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000);
		setf("lightY", ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000);
		setf("lightZ", ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000);
	}
	light.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));
		
	if(getb("selectionMode")) {
		cam.enableMouseInput();
	} else {
		updateRenderMode();
		cam.disableMouseInput();
	}
}

void enableFog(float nearFog, float farFog) {
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4]= {0, 0, 0, 1};
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogf(GL_FOG_START, nearFog);
	glFogf(GL_FOG_END, farFog);
}

void disableFog() {
	glDisable(GL_FOG);
}

void testApp::draw() {
	ofBackground(geti("backgroundColor"));
    if(getb("loadCalibration")) {
		loadCalibration();
		setb("loadCalibration", false);
	}
	if(getb("saveCalibration")) {
		saveCalibration();
		setb("saveCalibration", false);
	}
	if(getb("selectionMode")) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}
	if(!getb("validShader")) {
		ofPushStyle();
		ofSetColor(magentaPrint);
		ofSetLineWidth(8);
		ofLine(0, 0, ofGetWidth(), ofGetHeight());
		ofLine(ofGetWidth(), 0, 0, ofGetHeight());
		string message = "Shader failed to compile.";
		ofVec2f center(ofGetWidth(), ofGetHeight());
		center /= 2;
		center.x -= message.size() * 8 / 2;
		center.y -= 8;
		drawHighlightString(message, center);
		ofPopStyle();
	}
}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_LEFT || key == OF_KEY_UP || key == OF_KEY_RIGHT|| key == OF_KEY_DOWN){
		int choice = geti("selectionChoice");
		setb("arrowing", true);
		if(choice > 0){
			Point2f& cur = imagePoints[choice];
			switch(key) {
				case OF_KEY_LEFT: cur.x -= 1; break;
				case OF_KEY_RIGHT: cur.x += 1; break;
				case OF_KEY_UP: cur.y -= 1; break;
				case OF_KEY_DOWN: cur.y += 1; break;
			}
		}
	} else {
		setb("arrowing",false);
	}
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
	ofPushStyle();
	ofSetLineWidth(geti("lineWidth"));
	if(getb("useSmoothing")) {
		ofEnableSmoothing();
	} else {
		ofDisableSmoothing();
	}
	int shading = geti("shading");
	bool useLights = shading == 1;
	bool useShader = shading == 2;
	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}
	
	if(getb("highlight")) {
		objectMesh.clearColors();
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
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);
	if(useShader) {
		ofFile fragFile("shader.frag"), vertFile("shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
			bool validShader = shader.load("shader");
			setb("validShader", validShader);
		}
		lastFragTimestamp = fragTimestamp;
		lastVertTimestamp = vertTimestamp;
		
		shader.begin();
		shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
		shader.end();
	}
	ofColor transparentBlack(0, 0, 0, 0);
	switch(geti("drawMode")) {
		case 0: // faces
			if(useShader) shader.begin();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			objectMesh.drawFaces();
			if(useShader) shader.end();
			break;
		case 1: // fullWireframe
			if(useShader) shader.begin();
			objectMesh.drawWireframe();
			if(useShader) shader.end();
			break;
		case 2: // outlineWireframe
			LineArt::draw(objectMesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
		case 3: // occludedWireframe
			LineArt::draw(objectMesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
	}
	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();
}

void testApp::saveCalibration() {
	string dirName = "calibration-" + ofGetTimestampString() + "/";
	ofDirectory dir(dirName);
	dir.create();
	
	FileStorage fs(ofToDataPath(dirName + "calibration-advanced.yml"), FileStorage::WRITE);	
	
	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;
	
	double focalLength = intrinsics.getFocalLength();
	fs << "focalLength" << focalLength;
	
	Point2d fov = intrinsics.getFov();
	fs << "fov" << fov;
	
	Point2d principalPoint = intrinsics.getPrincipalPoint();
	fs << "principalPoint" << principalPoint;
	
	cv::Size imageSize = intrinsics.getImageSize();
	fs << "imageSize" << imageSize;
	
	fs << "translationVector" << tvec;
	fs << "rotationVector" << rvec;

	Mat rotationMatrix;
	Rodrigues(rvec, rotationMatrix);
	fs << "rotationMatrix" << rotationMatrix;
	
	double rotationAngleRadians = norm(rvec, NORM_L2);
	double rotationAngleDegrees = ofRadToDeg(rotationAngleRadians);
	Mat rotationAxis = rvec / rotationAngleRadians;
	fs << "rotationAngleRadians" << rotationAngleRadians;
	fs << "rotationAngleDegrees" << rotationAngleDegrees;
	fs << "rotationAxis" << rotationAxis;
	
	ofVec3f axis(rotationAxis.at<double>(0), rotationAxis.at<double>(1), rotationAxis.at<double>(2));
	ofVec3f euler = ofQuaternion(rotationAngleDegrees, axis).getEuler();
	Mat eulerMat = (Mat_<double>(3,1) << euler.x, euler.y, euler.z);
	fs << "euler" << eulerMat;
	
	ofFile basic("calibration-basic.txt", ofFile::WriteOnly);
	ofVec3f position( tvec.at<double>(1), tvec.at<double>(2));
	basic << "position (in world units):" << endl;
	basic << "\tx: " << ofToString(tvec.at<double>(0), 2) << endl;
	basic << "\ty: " << ofToString(tvec.at<double>(1), 2) << endl;
	basic << "\tz: " << ofToString(tvec.at<double>(2), 2) << endl;
	basic << "axis-angle rotation (in degrees):" << endl;
	basic << "\taxis x: " << ofToString(axis.x, 2) << endl;
	basic << "\taxis y: " << ofToString(axis.y, 2) << endl;
	basic << "\taxis z: " << ofToString(axis.z, 2) << endl;
	basic << "\tangle: " << ofToString(rotationAngleDegrees, 2) << endl;
	basic << "euler rotation (in degrees):" << endl;
	basic << "\tx: " << ofToString(euler.x, 2) << endl;
	basic << "\ty: " << ofToString(euler.y, 2) << endl;
	basic << "\tz: " << ofToString(euler.z, 2) << endl;
	basic << "fov (in degrees):" << endl;
	basic << "\thorizontal: " << ofToString(fov.x, 2) << endl;
	basic << "\tvertical: " << ofToString(fov.y, 2) << endl;
	basic << "principal point (in screen units):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	basic << "image size (in pixels):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	
	saveMat(Mat(objectPoints), dirName + "objectPoints.yml");
	saveMat(Mat(imagePoints), dirName + "imagePoints.yml");
}

void testApp::loadCalibration() {
    
    // retrieve advanced calibration folder
    
    string calibPath;
    ofFileDialogResult result = ofSystemLoadDialog("Select a calibration folder", true, ofToDataPath("", true));
    calibPath = result.getPath();
    
    // load objectPoints and imagePoints
    
    Mat objPointsMat, imgPointsMat;
    loadMat( objPointsMat, calibPath + "/objectPoints.yml");
    loadMat( imgPointsMat, calibPath + "/imagePoints.yml");
    
    int numVals;
    float x, y, z;
    cv::Point3f oP;
    
    const float* objVals = objPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=3) {
        oP.x = objVals[i];
        oP.y = objVals[i+1];
        oP.z = objVals[i+2];
        objectPoints[i/3] = oP;
    }
    
    cv::Point2f iP;
    
    referencePoints.resize( (imgPointsMat.cols * imgPointsMat.rows ) / 2, false);
    
    const float* imgVals = imgPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=2) {
        iP.x = imgVals[i];
        iP.y = imgVals[i+1];
        if(iP.x != 0 && iP.y != 0) {
            referencePoints[i/2] = true;
        }
        imagePoints[i/2] = iP;
    }
    
    
    // load the calibration-advanced yml
    
    FileStorage fs(ofToDataPath(calibPath + "/calibration-advanced.yml", true), FileStorage::READ);
    
    Mat cameraMatrix;
    Size2i imageSize;
    fs["cameraMatrix"] >> cameraMatrix;
    fs["imageSize"][0] >> imageSize.width;
    fs["imageSize"][1] >> imageSize.height;
    fs["rotationVector"] >> rvec;
    fs["translationVector"] >> tvec;
    
    intrinsics.setup(cameraMatrix, imageSize);
    modelMatrix = makeMatrix(rvec, tvec);
    
    calibrationReady = true;
}

void testApp::setupControlPanel() {
	panel.setup();
	panel.msg = "tab hides the panel, space toggles render/selection mode, 'f' toggles fullscreen.";
	
	panel.addPanel("Interaction");
	panel.addToggle("setupMode", true);
	panel.addSlider("scale", 1, .1, 25);
	panel.addSlider("backgroundColor", 0, 0, 255, true);
	panel.addMultiToggle("drawMode", 3, variadic("faces")("fullWireframe")("outlineWireframe")("occludedWireframe"));
	panel.addMultiToggle("shading", 0, variadic("none")("lights")("shader"));
	panel.addToggle("loadCalibration", false);
	panel.addToggle("saveCalibration", false);
	
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
	panel.addToggle("CV_CALIB_FIX_PRINCIPAL_POINT", false);
	
	panel.addPanel("Rendering");
	panel.addSlider("lineWidth", 2, 1, 8, true);
	panel.addToggle("useSmoothing", false);
	panel.addToggle("useFog", false);
	panel.addSlider("fogNear", 200, 0, 1000);
	panel.addSlider("fogFar", 1850, 0, 2500);
	panel.addSlider("screenPointSize", 2, 1, 16, true);
	panel.addSlider("selectedPointSize", 8, 1, 16, true);
	panel.addSlider("selectionRadius", 12, 1, 32);
	panel.addSlider("lightX", 200, -1000, 1000);
	panel.addSlider("lightY", 400, -1000, 1000);
	panel.addSlider("lightZ", 800, -1000, 1000);
	panel.addToggle("randomLighting", false);
	
	panel.addPanel("Internal");
	panel.addToggle("validShader", true);
	panel.addToggle("selectionMode", true);
	panel.addToggle("hoverSelected", false);
	panel.addSlider("hoverChoice", 0, 0, objectPoints.size(), true);
	panel.addToggle("selected", false);
	panel.addToggle("dragging", false);
	panel.addToggle("arrowing", false);
	panel.addSlider("selectionChoice", 0, 0, objectPoints.size(), true);
	panel.addSlider("slowLerpRate", .001, 0, .01);
	panel.addSlider("fastLerpRate", 1, 0, 1);
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
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);
	float w = ofGetWidth();
	float h = ofGetHeight();
	ofSetLineWidth(1.5);
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, geti("selectedPointSize"));
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}
	
void testApp::drawSelectionMode() {
	ofSetColor(255);
	cam.begin();
	float scale = getf("scale");
	ofScale(scale, scale, scale);
	if(getb("useFog")) {
		enableFog(getf("fogNear"), getf("fogFar"));
	}
	render();
	if(getb("useFog")) {
		disableFog();
	}
	if(getb("setupMode")) {
		imageMesh = getProjectedMesh(objectMesh);
	}
	cam.end();
	
	if(getb("setupMode")) {
		// draw all points cyan small
		glPointSize(geti("screenPointSize"));
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);
		imageMesh.drawVertices();

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
		if(!ofGetMousePressed() && distance < getf("selectionRadius")) {
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
	}
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
		if(getb("setupMode")) {
			imageMesh = getProjectedMesh(objectMesh);	
		}
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(getb("setupMode")) {
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
		} else if(getb("arrowing")) {
			Point2f& cur = imagePoints[choice];
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
        } else {
			// check to see if anything is selected
			// draw hover magenta
			float distance;
			ofVec2f selected = toOf(getClosestPoint(imagePoints, mouseX, mouseY, &choice, &distance));
			if(!ofGetMousePressed() && referencePoints[choice] && distance < getf("selectionRadius")) {
				seti("hoverChoice", choice);
				setb("hoverSelected", true);
				drawLabeledPoint(choice, selected, magentaPrint);
			} else {
				setb("hoverSelected", false);
			}
		}
	}
}
