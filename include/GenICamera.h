#pragma once
#include <boost/thread.hpp>
#include <iCVCDriver.h>
#include <iCVCUtilities.h>
#include "Module.h"

class GenICamera : public Module {
protected:
	bool is_running;
	IMG hCamera = NULL;
	boost::thread thread;

	int ocv_depth(cvbdatatype_t cvbDt) {
		// map compatible cvb data type descriptors to OpenCV data types 
		switch (cvbDt & 0xFF) {
		case 8: return IsSignedDatatype(cvbDt) ? CV_8S : CV_8U;
		case 16: return IsSignedDatatype(cvbDt) ? CV_16S : CV_16U;
		case 32: return IsFloatDatatype(cvbDt) ? CV_32F : CV_32S;
		case 64: return IsFloatDatatype(cvbDt) ? CV_64F : 0;
		default: return 0;
		}
	}

	void Capture() {
		cvbres_t result = G2Grab(hCamera);
		if (result < 0)
			return;
		cv::Mat image(cv::Size(ImageWidth(hCamera), ImageHeight(hCamera)), ocv_depth(ImageDatatype(hCamera, 0)));
		while (is_running) {
			result = G2Wait(hCamera);
			if (result >= 0) {
				void* ppixels = nullptr; intptr_t xInc = 0; intptr_t yInc = 0;
				GetLinearAccess(hCamera, 0, &ppixels, &xInc, &yInc);
				image.data = (uchar*)ppixels;
				pc_signal(image);
			}
		}
		result = G2Freeze(hCamera, true);
	}

public:
	GenICamera() :
		is_running(false) {
		// constant for path length
		static const size_t DRIVERPATHSIZE = 256;
		// load the first camera
		char driverPath[DRIVERPATHSIZE] = { 0 };
		TranslateFileName("%CVB%\\Drivers\\GenICam.vin", driverPath, DRIVERPATHSIZE);
		cvbbool_t success = LoadImageFile(driverPath, hCamera);
		if (!success) {
			std::stringstream errMsg;
			errMsg << "Error loading \"" << driverPath << "\" driver!";
			throw std::exception(errMsg.str().c_str());
		}
	}

	~GenICamera() {
		ReleaseObject(hCamera);
	}

	virtual void operator()(const cv::Mat& image) {
	}

	void Start() {
		thread = boost::thread(&GenICamera::Capture, this);
		is_running = true;
	}

	void Stop() {
		is_running = false;
		thread.join();
	}

	bool IsRunning() {
		return is_running;
	}

};

