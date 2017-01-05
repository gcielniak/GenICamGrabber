#pragma once
#include <boost/thread.hpp>
#include <iCVCDriver.h>
#include <iCVCUtilities.h>
#include "Module.h"

class GenICamera : public Module {
protected:
	bool is_running, initialised;
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
		//start grabbing
		cv::Mat image(cv::Size(ImageWidth(hCamera), ImageHeight(hCamera)), ocv_depth(ImageDatatype(hCamera, 0)));
		double t_start = 0.0, t_now;
		while (is_running) {
			if (G2Wait(hCamera) >= 0) {
				//assign newly captured buffer to OpenCV Mat object
				void* ppixels = nullptr; intptr_t xInc = 0; intptr_t yInc = 0;
				GetLinearAccess(hCamera, 0, &ppixels, &xInc, &yInc);
				image.data = (uchar*)ppixels;
				//calculate timestamp in nanoseconds
				G2GetGrabStatus(hCamera, GRAB_INFO_CMD::GRAB_INFO_TIMESTAMP, t_now);
				if (t_start == 0.0)
					t_start = t_now;
				long long timestamp = (long long)(t_now - t_start);
				pc_signal(image, timestamp);
			}
		}
	}

public:
	GenICamera() :
		is_running(false), initialised(false) {
	}

	~GenICamera() {
		Stop();
		ReleaseObject(hCamera);
	}

	virtual void operator()(const cv::Mat& image, long long timestamp) {
	}

	void Init() {
		if (!initialised) {
			static const size_t DRIVERPATHSIZE = 256;
			// load the first camera
			char driverPath[DRIVERPATHSIZE] = { 0 };
			TranslateFileName("%CVB%\\Drivers\\GenICam.vin", driverPath, DRIVERPATHSIZE);
			if (!LoadImageFile(driverPath, hCamera))
				throw LibException("GenICamera::Init, Error loading \"" + std::string(driverPath) + "\" driver!");
			initialised = true;
		}
	}

	void Start() {
		Init();
		if (G2Grab(hCamera) < 0) {
			throw LibException("GenICamera::Capture, G2Grab function failure.");
			return;
		}
		thread = boost::thread(&GenICamera::Capture, this);
		is_running = true;
	}

	void Stop() {
		is_running = false;
		thread.join();
		G2Freeze(hCamera, true);
	}

	bool IsRunning() {
		return is_running;
	}
};

