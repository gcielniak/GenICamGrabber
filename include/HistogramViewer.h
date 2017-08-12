#pragma once

#include <string>
#include <vector>
#include "SignalConnector.h"

using namespace std;

class HistogramViewer : public ImageSignalConnector {
protected:
	std::string window_name;
	bool init;

public:

	HistogramViewer(const std::string name = "") :
		window_name(name), init(true) {
	}

	void CumSum(cv::Mat& image) {
		for (int i = 1; i < image.rows; ++i)
			for (int j = 0; j < image.cols; ++j)
				image.at<float>(i, j) += image.at<float>(i - 1, j);
	}

	virtual void operator()(const Image& image) {
		if (init) {
			cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
//			cv::resizeWindow(window_name, image.cols / 2, image.rows / 2);
			init = false;
		}

		/// Separate the image in 3 places ( B, G and R )
		std::vector<cv::Mat> bgr_planes;
		split(image.image, bgr_planes);

		/// Establish the number of bins
		int histSize = 256;

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 };
		const float* histRange = { range };

		bool uniform = true; 
		bool cumulative = false;
		bool accumulate = false;

		cv::Mat b_hist, g_hist, r_hist, n_hist;

		/// Compute the histograms:
		calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[3], 1, 0, cv::Mat(), n_hist, 1, &histSize, &histRange, uniform, accumulate);

		///cumulative histogram
		if (cumulative) {
			CumSum(b_hist);
			CumSum(g_hist);
			CumSum(r_hist);
			CumSum(n_hist);
		}

		// Draw the histograms for B, G and R
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / histSize);

		cv::Mat histImage(hist_h, hist_w, CV_8UC4, cv::Scalar(0, 0, 0));

		/// Normalize the result to [ 0, histImage.rows ]
		normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
		normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
		normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
		normalize(n_hist, n_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

		/// Draw for each channel
		for (int i = 1; i < histSize; i++)
		{
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
				cv::Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
				cv::Scalar(255, 0, 0), 2, 8, 0);
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
				cv::Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
				cv::Scalar(0, 255, 0), 2, 8, 0);
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
				cv::Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
				cv::Scalar(0, 0, 255), 2, 8, 0);
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(n_hist.at<float>(i - 1))),
				cv::Point(bin_w*(i), hist_h - cvRound(n_hist.at<float>(i))),
				cv::Scalar(0, 255, 255), 2, 8, 0);
		}

		cv::imshow(window_name, histImage);
		cv::waitKey(1);
	}
};

