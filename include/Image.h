#pragma once

#include <opencv2/opencv.hpp>

enum ImageType {
	RGB_IMAGE,
	MONO_IMAGE,
	DEPTH_IMAGE,
	NIR_IMAGE,
	RGBN_IMAGE,
	STEREOL_IMAGE,
	STEREOR_IMAGE
};

///
/// Basic Image class representing different formats and additional information (e.g. timestamp)
///

struct Image {
	ImageType type;
	cv::Mat image;
	long long timestamp;
	Image(const cv::Mat& image_, long long timestamp_, ImageType type_) : 
		image(image_), timestamp(timestamp_), type(type_) {
	}
};

///
/// Colour image
///

struct RGBImage : public Image {
	RGBImage(const cv::Mat& image_=cv::Mat(), long long timestamp_=0) :
		Image(image_, timestamp_, RGB_IMAGE) {
	}
};

///
/// Mono image
///

struct MonoImage : public Image {
	MonoImage(const cv::Mat& image_ = cv::Mat(), long long timestamp_ = 0) :
		Image(image_, timestamp_, MONO_IMAGE) {
	}
};

///
/// NIR image
///

struct NIRImage : public Image {
	NIRImage(const cv::Mat& image_ = cv::Mat(), long long timestamp_ = 0) :
		Image(image_, timestamp_, NIR_IMAGE) {
	}
};

///
/// 4 channel RGB and NIR image
///

struct RGBNImage : public Image {
	RGBNImage(const cv::Mat& image_ = cv::Mat(), long long timestamp_ = 0) :
		Image(image_, timestamp_, RGBN_IMAGE) {
	}
};
