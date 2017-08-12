#pragma once

#include "Image.h"
#include "SignalConnector.h"

class RGBN2NDVI : public ImageSignalConnector {
public:
	virtual void operator()(const Image& image) {
		//NIR - RED
		//NIR + RED

		cv::Mat red, nir, ndvi_8U;
		std::vector<cv::Mat> rgbn;
		cv::split(image.image, rgbn);

		rgbn[0].convertTo(red, CV_32F);
		rgbn[3].convertTo(nir, CV_32F);

		cv::Mat ndvi = ((nir - red) / (nir + red) + 1.0)/2;

		signal(MonoImage(ndvi, image.timestamp));
	}
};
