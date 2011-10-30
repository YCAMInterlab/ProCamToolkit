#pragma once

#include "opencv2/opencv.hpp"

//
// This code was ported from Matlab code provided in the following paper:
//
// P. E. Debevec and J. Malik, "Recovering High Dynamic Range Radiance Maps from
// Photographs," Proceedings of SIGGRAPH 1997, ACM Press / ACM SIGGRAPH, 369-
// 378, 1997.
//

//
// gsolve.m - Solve for imaging system response function
//
// Given a set of pixel values observed for several pixels in several
// images with different exposure times, this function returns the
// imaging system?s response function g as well as the log film irradiance
// values for the observed pixels.
//
// Assumes:
//
//	Zmin = 0
//	Zmax = 255
//
// Arguments:
//
//	Z(i,j) is the pixel values of pixel location number i in image j
//	B(j) is the log delta t, or log shutter speed, for image j
//	l is lamdba, the constant that determines the amount of smoothness
//	w(z) is the weighting function value for pixel value z
//
// Returns:
//
//	g(z) is the log exposure corresponding to pixel value z
//	lE(i) is the log film irradiance at pixel location i
//

void gsolve(const cv::Mat& Z, const cv::Mat& B, float l, const cv::Mat& w,
		cv::Mat& g, cv::Mat& lE);
		
cv::Mat buildWeights();