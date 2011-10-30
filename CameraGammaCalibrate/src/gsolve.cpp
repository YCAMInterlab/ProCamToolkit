#include "gsolve.hpp"

using namespace cv;
#define db at<double>
#define uc at<unsigned char>

void gsolve(const Mat& Z, const Mat& B, float l, const Mat& w,
		Mat& g, Mat& lE) {
	const int n = 256;
	
	Mat A = Mat::zeros(Z.rows * Z.cols + n - 1, n + Z.rows, CV_64FC1);
	Mat b = Mat::zeros(A.rows, 1, CV_64FC1);
	
	// Include the data-fitting equations
	int k = 0;
	for(int i = 0; i < Z.rows; i++) {
		for(int j = 0; j < Z.cols; j++) {
			unsigned char curZ = Z.uc(i, j);
			float wij = w.db(curZ);
			A.db(k, curZ) = wij;
			A.db(k, n + i) = -wij;
			b.db(k) = wij * B.db(j);
			k++;
		}
	}
	
	// Fix the curve by setting its middle value to 0
	A.db(k, 128) = 1;
	k++;
	
	// Include the smoothness equations
	for(int i = 1; i < n - 1; i++) {
		A.db(k, i - 1) = l * w.db(i);
		A.db(k, i + 0) = l * w.db(i) * -2;
		A.db(k, i + 1) = l * w.db(i);
		k++;
	}
	
	// Solve the system using SVD
	Mat x = Mat::zeros(A.cols, 1, CV_64FC1);
	solve(A, b, x, CV_SVD);
	
	g = x.rowRange(0, n).clone();
	exp(g, g);
	lE = x.rowRange(n, x.rows).clone();
}

Mat buildWeights() {
	const int n = 256;
	
	Mat w = Mat::zeros(n, 1, CV_64FC1);
	const int zmax = n - 1;
	const int zmin = 0;
	const int zmid = (zmin + zmax) / 2;
	for(int z = 0; z < zmid; z++) {
		w.db(z) = z - zmin;
	}
	for(int z = zmid; z < n; z++) {	
		w.db(z) = zmax - z;
	}
	return w;
}
