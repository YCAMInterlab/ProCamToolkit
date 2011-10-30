#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024, 1024, OF_WINDOW);
	ofRunApp(new testApp());
}
