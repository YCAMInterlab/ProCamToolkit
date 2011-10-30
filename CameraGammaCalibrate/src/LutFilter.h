#pragma once

#include <fstream>
#include "ofMain.h"

class LutFilter {
public:
	LutFilter();
	void setup(string filename);
	void filter(ofImage& image);

private:
	static const int levels = 256;

	int type;
	unsigned char grayGamma[levels];
	unsigned char rgbGamma[levels][3];

	static void tokenize(const string& str, vector<string>& tokens, const string& delimiters);
	static int toInt(string str);
};
