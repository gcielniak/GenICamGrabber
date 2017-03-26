#pragma once
#include <boost/thread.hpp>
#include "Module.h"
#include "GenICamDevice.h"

class GenICamGrabber : public Module, public GenICamDevice {
protected:
	bool is_running = false;
	bool initialised = false;
	boost::thread thread;
	int type;
	cv::Mat image, image_mono16;

	void Capture() {
		//start grabbing
		while (is_running) {
			this->TriggerSoftware();
			image.data = this->GetBuffer();
			if (type == CV_8UC3) {
				cv::cvtColor(image, image, CV_BGR2RGB);
				pc_signal(image, GetTimeStamp());
			}
			else {
				image.convertTo(image_mono16, CV_16UC1);
				image_mono16 *= 256;
				pc_signal(image_mono16, GetTimeStamp());
			}
		}
	}

public:
	GenICamGrabber() {
	}

	~GenICamGrabber() {
		Stop();
	}

	virtual void operator()(const cv::Mat& image, long long timestamp) {
	}

	void Init() {
		GenICamDevice::Init();
	}

	void Start() {
		if (!is_running) {
			GenICamDevice::Start();

			thread = boost::thread(&GenICamGrabber::Capture, this);
			is_running = true;

			int width, height, channels, pixel_depth;
			this->GetImageProps(width, height, channels, pixel_depth);

			if (channels == 3)
				type = CV_8UC3;
			else
				type = CV_8U;

			image = cv::Mat(cv::Size(width, height), type);
			image_mono16 = cv::Mat(cv::Size(width, height), CV_16UC1);
		}
	}

	void Stop() {
		is_running = false;
		initialised = false;
		thread.join();
		GenICamDevice::Stop();
	}

	bool IsRunning() {
		return is_running;
	}
};
