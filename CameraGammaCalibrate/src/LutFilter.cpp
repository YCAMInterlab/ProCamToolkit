#include "LutFilter.h"

LutFilter::LutFilter() {
}

void LutFilter::setup(string filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str());
	if(file.is_open()) {
		string line;
		int level = 0;
		while (!file.eof()) {
			getline(file, line);
			vector<string> split;
			tokenize(line, split, ",");
			if(split.size() == 3) {
				type = OF_IMAGE_COLOR;
				for(int i = 0; i < 3; i++)
					rgbGamma[level][i] = toInt(split[i]);
			} else if(split.size() == 1) {
				type = OF_IMAGE_GRAYSCALE;
				grayGamma[level] = toInt(split[0]);
			}
			level++;
		}
		file.close();
	}
}

void LutFilter::tokenize(const string& str, vector<string>& tokens, const string& delimiters) {
	int lastPos = str.find_first_not_of(delimiters, 0);
	int pos = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

int LutFilter::toInt(string str) {
	stringstream stream;
	stream << str;
	int out;
	stream >> out;
	return out;
}

void LutFilter::filter(ofImage& image) {
	if(type != image.type) {
		ofLog(OF_LOG_ERROR, "Image type does not match LUT type.");
		return;
	}
	int n = (int) image.getWidth() * (int) image.getHeight();
	unsigned char* pixels = image.getPixels();
	if(type == OF_IMAGE_COLOR) {
		unsigned char* end = &pixels[n * 3];
		while(pixels != end) {
			*(pixels++) = rgbGamma[*pixels][0];
			*(pixels++) = rgbGamma[*pixels][1];
			*(pixels++) = rgbGamma[*pixels][2];
		}
	} else if(type == OF_IMAGE_GRAYSCALE) {
		unsigned char* end = &pixels[n];
		while(pixels != end)
			*(pixels++) = grayGamma[*pixels];
	}
}
