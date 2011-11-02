#include "PatternGenerator.h"

void PatternGenerator::allocateSequence(int n) {
	sequence.clear();
	sequence.resize(n);
	for(int i = 0; i < n; i++) {
		sequence[i].allocate(width, height, OF_IMAGE_COLOR);
	}
}

PatternGenerator::PatternGenerator() :
	width(0), height(0),
	reverse(false) {
}

void PatternGenerator::setSize(int width, int height) {
	this->width = width;
	this->height = height;
}

void PatternGenerator::setReverse(bool reverse) {
	this->reverse = reverse;
}

void PatternGenerator::setInverse(bool inverse) {
	this->inverse = inverse;
}

int PatternGenerator::getWidth() {
	return width;
}

int PatternGenerator::getHeight() {
	return height;
}

int PatternGenerator::size() {
	return sequence.size();
}

ofImage& PatternGenerator::get(int i) {
	i %= size();
	if(reverse) {
		return sequence[size() - i - 1];
	} else {
		return sequence[i];
	}
}
