#include "ThreadedSolver.h"

ThreadedSolver::ThreadedSolver() {
}

void ThreadedSolver::setup() {
	directory = "firefly";
	string extension = ".png";
	int sampledshutters = 64;
	sampledLocations = 64;
	smoothness = .1;

	#ifdef DOUBLECHECK
	lut.setup("firefly-lut.csv");
	#endif

	ofxDirList dirList;
	int n = dirList.listDir(directory);
	cout << n << " images available." << endl;

	vector<int> shutters;
	for(int i = 0; i < n; i++) {
		string curFile = dirList.getName(i);
		stringstream name;
		name << curFile.substr(0, curFile.rfind("."));
		int shutter;
		name >> shutter;
		shutters.push_back(shutter);
	}

	sort(shutters.begin(), shutters.end());

	cout << "Loading images from " << directory << "/ :";
	for(int i = 0; i < sampledshutters; i++) {
		int j = (i * n) / sampledshutters;
		int curshutter = shutters[j];
		cout << " " << j << "/" << curshutter;
		images.push_back(ofImage());
		images.back().loadImage(directory + "/" + ofToString(curshutter) + extension);
		#ifdef DOUBLECHECK
		lut.filter(images.back());
		#endif
		shutterTimes.push_back(curshutter);
	}
	cout << endl;
	cout << "Loaded " << images.size() << " images." << endl;

	ofImage prototype;
	prototype.loadImage(dirList.getPath(0));
	type = prototype.type;

	if(type == OF_IMAGE_COLOR) {
		redGammaSum.resize(levels);
		greenGammaSum.resize(levels);
		blueGammaSum.resize(levels);
		cout << "Computing color gamma." << endl;
	} else if(type == OF_IMAGE_GRAYSCALE) {
		grayGammaSum.resize(levels);
		cout << "Computing grayscale gamma." << endl;
	}

	iterations = 0;

	ready = false;

	startThread();
}

void ThreadedSolver::threadedFunction() {
	while(true) {
		iterations++;

		// iteratively compute gamma
		vector<float> curRedGamma, curGreenGamma, curBlueGamma;
		vector<float> curGrayGamma;
		cout << "Computing iteration " << iterations << endl;
		if(type == OF_IMAGE_COLOR) {
			GammaCalibration::calibrate(images, shutterTimes,
				curRedGamma, curGreenGamma, curBlueGamma,
				sampledLocations, smoothness);
		} else if(type == OF_IMAGE_GRAYSCALE) {
			GammaCalibration::calibrate(images, shutterTimes,
				curGrayGamma,
				sampledLocations, smoothness);
		}

		ready = false;

		// sum gammas
		for(int i = 0; i < levels; i++) {
			if(type == OF_IMAGE_COLOR) {
				if(isnan(curRedGamma[i]) || isnan(curGreenGamma[i]) || isnan(curBlueGamma[i])) {
					cout << "Not enough data." << endl;
					exit(1);
				}
				redGammaSum[i] += curRedGamma[i];
				greenGammaSum[i] += curGreenGamma[i];
				blueGammaSum[i] += curBlueGamma[i];
			} else if(type == OF_IMAGE_GRAYSCALE) {
				if(isnan(curGrayGamma[i])) {
					cout << "Not enough data." << endl;
					exit(1);
				}
				grayGammaSum[i] += curGrayGamma[i];
			}
		}

		// average gamma
		if(type == OF_IMAGE_COLOR) {
			redGamma.clear();
			greenGamma.clear();
			blueGamma.clear();
		} else if(type == OF_IMAGE_GRAYSCALE) {
			grayGamma.clear();
		}
		for(int i = 0; i < levels; i++) {
			if(type == OF_IMAGE_COLOR) {
				redGamma.push_back(redGammaSum[i] / iterations);
				greenGamma.push_back(greenGammaSum[i] / iterations);
				blueGamma.push_back(blueGammaSum[i] / iterations);
			} else if(type == OF_IMAGE_GRAYSCALE) {
				grayGamma.push_back(grayGammaSum[i] / iterations);
			}
		}

		// write updated gamma
		ofstream curves;
		string filename = "curves-" + directory + "-" + ofToString(iterations) + ".csv";
		curves.open(filename.c_str());
		for(int i = 0; i < levels; i++) {
			if(type == OF_IMAGE_COLOR) {
				curves << redGamma[i] << "," << greenGamma[i]  << "," << blueGamma[i] << endl;
			} else if(type == OF_IMAGE_GRAYSCALE) {
				curves << grayGamma[i] << endl;
			}
		}
		curves.flush();
		curves.close();

		ready = true;
	}
}
