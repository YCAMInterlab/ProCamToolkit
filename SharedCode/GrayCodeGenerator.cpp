#include "GrayCodeGenerator.h"

GrayCodeGenerator::GrayCodeGenerator() :
	orientation(VERTICAL) {
}

void GrayCodeGenerator::setOrientation(Orientation orientation) {
	this->orientation = orientation;
}

// for conversion: http://www.dspguru.com/dsp/tricks/gray-code-conversion
#define makeGrayCode(x) (x >> 1) ^ x
#define isTrue(x, i) (x >> i & 1)

void GrayCodeGenerator::generate() {
	int length = orientation == VERTICAL ? width : height;
	int subdivisions = (int) ceil(log2(length));
	cout << "building with " << length << "/" << subdivisions << endl;
	allocateSequence(subdivisions);
	unsigned char bright = inverse ? 0 : 255;
	unsigned char dark = inverse ? 255 : 0;
	for(int k = 0; k < subdivisions; k++) {
		unsigned char* single = new unsigned char[length * 3];
		int i = 0;
		for(unsigned int j = 0; j < length; j++) {
			unsigned int grayCode = makeGrayCode(j);
			unsigned char value = isTrue(grayCode, k) ? bright : dark;
			single[i++] = value;
			single[i++] = value;
			single[i++] = value;
		}
		int sequencePosition = subdivisions - k - 1;
		unsigned char* pixels = sequence[sequencePosition].getPixels();
		if(orientation == VERTICAL) {
			for(int y = 0; y < height; y++) {
				memcpy(&pixels[y * width * 3], single, width * 3);
			}
		} else {
			int i = 0;
			int j = 0;
			for(int y = 0; y < height; y++) {
				for(int x = 0; x < width; x++) {
					memcpy(&pixels[i], &single[j], 3);
					i += 3;
				}
				j += 3;
			}
		}
		delete [] single;
		sequence[sequencePosition].update();
	}
}
