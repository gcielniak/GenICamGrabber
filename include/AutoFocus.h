#pragma once
#include <limits>
#include <iostream>
#include "SignalConnector.h"

using namespace std;

class AutoFocus : public ImageSignalConnector {
	double focus_min, focus_max, focus_current, alpha;

public:
	AutoFocus() : focus_min(100000), focus_max(0), alpha(0.5), focus_current(0) {
	}

	virtual void operator()(const Image& image) {
		std::vector<cv::Mat> channels;
		cv::split(image.image, channels); // break image into channels
		int cx = image.image.cols / 2;
		int cy = image.image.rows / 2;
		int region_size = 64;
		cv::Mat roi = channels[0](cv::Range(cy - region_size, cy + region_size), cv::Range(cx - region_size, cx + region_size));

		double focus_raw = tenengrad(roi, 5);
		focus_min = std::min<double>(focus_min, focus_raw);
		focus_max = std::max<double>(focus_max, focus_raw);
		focus_current = focus_raw*alpha + focus_current*(1 - alpha);

		double focus_norm = (focus_current - focus_min) / (focus_max - focus_min);

//		cerr << "min: " << focus_min << " max: " << focus_max << " current: " << focus_current << endl;
		cerr << "autofocus: " << fixed << setprecision(2) << focus_norm << endl;

	}

	// OpenCV port of 'LAPM' algorithm (Nayar89)
	double modifiedLaplacian(const cv::Mat& src)
	{
		cv::Mat M = (cv::Mat_<double>(3, 1) << -1, 2, -1);
		cv::Mat G = cv::getGaussianKernel(3, -1, CV_64F);

		cv::Mat Lx;
		cv::sepFilter2D(src, Lx, CV_64F, M, G);

		cv::Mat Ly;
		cv::sepFilter2D(src, Ly, CV_64F, G, M);

		cv::Mat FM = cv::abs(Lx) + cv::abs(Ly);

		double focusMeasure = cv::mean(FM).val[0];
		return focusMeasure;
	}

	// OpenCV port of 'LAPV' algorithm (Pech2000)
	double varianceOfLaplacian(const cv::Mat& src)
	{
		cv::Mat lap;
		cv::Laplacian(src, lap, CV_64F);

		cv::Scalar mu, sigma;
		cv::meanStdDev(lap, mu, sigma);

		double focusMeasure = sigma.val[0] * sigma.val[0];
		return focusMeasure;
	}

	// OpenCV port of 'TENG' algorithm (Krotkov86)
	double tenengrad(const cv::Mat& src, int ksize)
	{
		cv::Mat Gx, Gy;
		cv::Sobel(src, Gx, CV_64F, 1, 0, ksize);
		cv::Sobel(src, Gy, CV_64F, 0, 1, ksize);

		cv::Mat FM = Gx.mul(Gx) + Gy.mul(Gy);

		double focusMeasure = cv::mean(FM).val[0];
		return focusMeasure;
	}

	// OpenCV port of 'GLVN' algorithm (Santos97)
	double normalizedGraylevelVariance(const cv::Mat& src)
	{
		cv::Scalar mu, sigma;
		cv::meanStdDev(src, mu, sigma);

		double focusMeasure = (sigma.val[0] * sigma.val[0]) / mu.val[0];
		return focusMeasure;
	}
};


