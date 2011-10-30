#include "GammaCalibration.h"

void GammaCalibration::calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& grayGamma,
		int sampledLocations, float smoothness) {
	calibrate(images, shutterTimes, grayGamma, 0, 1, sampledLocations, smoothness);
}

void GammaCalibration::calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& redGamma, vector<float>& greenGamma, vector<float>& blueGamma,
		int sampledLocations, float smoothness) {
	calibrate(images, shutterTimes, redGamma, 0, 3, sampledLocations, smoothness);
	calibrate(images, shutterTimes, greenGamma, 1, 3, sampledLocations, smoothness);
	calibrate(images, shutterTimes, blueGamma, 2, 3, sampledLocations, smoothness);
}

void GammaCalibration::calibrate(vector<ofImage>& images, vector<float>& shutterTimes,
		vector<float>& gamma,
		int offset, int step,
		int sampledLocations, float smoothness) {
	// take the log of the shutterTimes
	int sampledshutters = images.size();
	CvMat* inputshutters = cvCreateMat(sampledshutters, 1, CV_32FC1);
	float* inputshuttersPtr = (float*) cvPtr1D(inputshutters, 0);
	for(int i = 0; i < shutterTimes.size(); i++)
		inputshuttersPtr[i] = logf(shutterTimes[i]);

	// allocate working space
	CvMat* inputCurves = cvCreateMat(sampledLocations, sampledshutters, CV_8UC1);
	CvMat* weighting = cvCreateMat(levels, 1, CV_8UC1);
	int* locations = new int[sampledLocations];

	// allocate the linear system
	long sizeA1 = sampledLocations * sampledshutters + levels + 1;
	long sizeA2 = levels + sampledLocations;
	CvMat* A = cvCreateMat(sizeA1, sizeA2, CV_32FC1);
	CvMat* b = cvCreateMat(sizeA1, 1, CV_32FC1);
	CvMat* x = cvCreateMat(sizeA2, 1, CV_32FC1);
	float* xptr = (float*) cvPtr1D(x, 0);

	// histogram the intensity values of the middle image
	ofImage& middleImage = images[images.size() / 2];
	vector< vector<int> > histogram(levels);
	int totalPixels = (int) middleImage.getWidth() * (int) middleImage.getHeight();
	unsigned char* middle = middleImage.getPixels();
	for(int i = 0; i < totalPixels; i++)
		histogram[middle[i * step + offset]].push_back(i);

	// find nonempty histogram bins
	vector<unsigned char> nonempty;
	for(int i = 0; i < levels; i++)
		if(histogram[i].size() > 0)
			nonempty.push_back(i);

	// from nonempty bins, pick random shutter-equidistant locations
	for(int i = 0; i < sampledLocations; i++) {
		int intensity = nonempty[(i * nonempty.size()) / sampledLocations];
		vector<int>& bin = histogram[intensity];
		locations[i] = bin[(int) ofRandom(0, bin.size())];
	}

	// given the locations, fill up the input curves
	unsigned char* inputCurvesPtr = (unsigned char*) cvPtr2D(inputCurves, 0, 0);
	for(int i = 0; i < sampledLocations; i++) {
		int curLocation = locations[i] * step + offset;
		for(int j = 0; j < sampledshutters; j++) {
			unsigned char* curPixels = images[j].getPixels();
			*(inputCurvesPtr++) = curPixels[curLocation];
		}
	}

	// generate the weighting (hat) function
	unsigned char* weightingPtr = (unsigned char*) cvPtr1D(weighting, 0);
	for(int z = 0; z < levels / 2; z++)
		weightingPtr[z] = z;
	for(int z = levels / 2; z < levels; z++)
		weightingPtr[z] = levels - z;

	// fill up the system: data fitting
	int k = 0;
	for(int i = 0; i < sampledLocations; i++) {
		for(int j = 0; j < sampledshutters; j++) {
			unsigned char curZ = *cvPtr2D(inputCurves, i, j);
			unsigned char wij = weightingPtr[curZ];
			*((float*) cvPtr2D(A, k, curZ)) = wij;
			*((float*) cvPtr2D(A, k, levels + i)) = -wij;
			*((float*) cvPtr1D(b, k)) = wij * inputshuttersPtr[j];
			k++;
		}
	}

	// fill up the system: set the middle of the curve to 0
	*((float*) cvPtr2D(A, k, 128)) = 0;
	k++;

	// fill up the system: include smoothness constraint
	for(int i = 0; i < levels - 2; i++) {
		*((float*) cvPtr2D(A, k, i)) = smoothness * weightingPtr[i];
		*((float*) cvPtr2D(A, k, i + 1)) = -2 * smoothness * weightingPtr[i];
		*((float*) cvPtr2D(A, k, i + 2)) = smoothness * weightingPtr[i];
		k++;
	}

	// solve the system
	cvSolve(A, b, x, CV_SVD);

	gamma.clear();
	gamma.resize(256);

	// load gamma from xptr
	for(int i = 0; i < levels; i++) {
		float cur = expf(xptr[i]);
		gamma[i] += cur;
	}

	// normalize to range
	float top = gamma[255];
	for(int i = 0; i < levels; i++)
		gamma[i] = (gamma[i] * 255) / top;
	//gamma[0] = 0;

	// clear all the allocated memory
	delete [] locations;

	cvReleaseMat(&A);
	cvReleaseMat(&b);
	cvReleaseMat(&x);

	cvReleaseMat(&inputCurves);
	cvReleaseMat(&inputshutters);
	cvReleaseMat(&weighting);
}
