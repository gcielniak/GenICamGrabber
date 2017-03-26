#pragma once
#include <string>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include "Module.h"

class FPSCounter {
	typedef boost::chrono::duration<double> sec;

	boost::timer::cpu_timer timer;

public:
	void Update() {
		static boost::timer::cpu_times t_start = timer.elapsed();
		static unsigned int count = 0;
		boost::timer::cpu_times t_now = timer.elapsed();

		sec t_start_sec = boost::chrono::nanoseconds(t_start.user);
		sec t_now_sec = boost::chrono::nanoseconds(t_now.user);
		++count;
		double diff = t_now_sec.count() - t_start_sec.count();
		if (diff >= 1.0) {
			std::cerr << "Avg. fps: " << count / diff << std::endl;
			t_start = t_now;
			count = 0;
		}
	}
};

class ImageViewer : public Module {
protected:
	std::string window_name;
	bool is_running;
	bool init;
	boost::mutex mutex;
	FPSCounter fps_counter;

public:
	ImageViewer(const std::string name = "") :
		window_name(name), is_running(true), init(true) {
	}

	virtual void operator()(const cv::Mat& image, long long timestamp) {
		if (init) {
			cv::namedWindow(window_name, cv::WINDOW_NORMAL);
			cv::resizeWindow(window_name, image.cols / 4, image.rows / 4);
			init = false;
		}

		cv::imshow(window_name, image);
		if (cv::waitKey(2) > 0) {
			boost::unique_lock<boost::mutex> lock(mutex);
			is_running = false;
			lock.unlock();
		}

		fps_counter.Update();
	}

	bool IsRunning() {
		boost::unique_lock<boost::mutex> lock(mutex);
		return is_running;
		lock.unlock();
	}
};

