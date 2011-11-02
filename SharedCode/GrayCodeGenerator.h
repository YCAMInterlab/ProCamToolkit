#pragma once

#include "PatternGenerator.h"

class GrayCodeGenerator : public PatternGenerator {
private:
	Orientation orientation;
public:
	GrayCodeGenerator();
	void setOrientation(Orientation orientation);
	void generate();
};
