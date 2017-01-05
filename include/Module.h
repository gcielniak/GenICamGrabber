#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>
#include <opencv2/opencv.hpp>

class Module {
protected:
	boost::signals2::signal<void(const cv::Mat&)> pc_signal;

public:
	virtual void operator()(const cv::Mat&) = 0;

	void Connect(Module& module) {
		pc_signal.connect(bind(&Module::operator(), &module, _1));
	}
};

