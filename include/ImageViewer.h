#pragma once
#include <string>
#include <mutex>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include "SignalConnector.h"
#include "Image.h"


class ImageViewer : public ImageSignalConnector {
protected:
	std::string window_name;
	bool is_running;
	bool init;
	std::mutex mutex;

public:
	boost::signals2::signal<void(int)> keyboard_signal;

	ImageViewer(const std::string name = "") :
		window_name(name), is_running(true), init(true) {
	}

	virtual void operator()(const Image& image) {
		if (init) {
			cv::namedWindow(window_name, cv::WINDOW_NORMAL);
			cv::resizeWindow(window_name, image.image.cols / 2, image.image.rows / 2);
			init = false;
		}

		cv::imshow(window_name, image.image);
		int key = cv::waitKey(1);
		if (key == 27) {
			std::unique_lock<std::mutex> lock(mutex);
			is_running = false;
			lock.unlock();
		}
		else if (key > 0) {
			keyboard_signal(key);
		}
	}

	bool IsRunning() {
		std::unique_lock<std::mutex> lock(mutex);
		return is_running;
		lock.unlock();
	}
};

