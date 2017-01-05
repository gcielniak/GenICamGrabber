#pragma once
#include <string>
#include <boost/thread.hpp>
#include "Module.h"

class ImageViewer : public Module {
protected:
	std::string window_name;
	bool is_running;
	bool init;
	boost::mutex mutex;

public:
	ImageViewer(const std::string name = "") :
		window_name(name), is_running(true), init(true) {
	}

	virtual void operator()(const cv::Mat& image) {
		if (init) {
			cv::namedWindow(window_name, cv::WINDOW_NORMAL);
			cv::resizeWindow(window_name, image.cols / 4, image.rows / 4);
			init = false;
		}
		cv::imshow(window_name, image);
		if (cv::waitKey(30) > 0) {
			boost::unique_lock<boost::mutex> lock(mutex);
			is_running = false;
			lock.unlock();
		}
	}

	bool IsRunning() {
		boost::unique_lock<boost::mutex> lock(mutex);
		return is_running;
		lock.unlock();
	}
};

