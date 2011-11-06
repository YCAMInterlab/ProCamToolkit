#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, testApp::tw, testApp::th / 2, OF_WINDOW);
	ofRunApp(new testApp());
}
