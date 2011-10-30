#pragma once

#include "ofMain.h"
#include "ThreadedSolver.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	ThreadedSolver solver;
};
