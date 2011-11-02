#pragma once

#include "ofMain.h"
#include <fstream>

class PatternGenerator {
protected:
	bool reverse, inverse;
	int width, height;
	vector<ofImage> sequence;
	void allocateSequence(int n);
public:
	enum Orientation {VERTICAL, HORIZONTAL};

	PatternGenerator();
	void setSize(int width, int height);
	void setReverse(bool reverse);
	void setInverse(bool inverse);
	virtual void generate() = 0;
	int getWidth();
	int getHeight();
	int size();
	ofImage& get(int i);
};
