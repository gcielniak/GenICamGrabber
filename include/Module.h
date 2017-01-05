#pragma once
#include <exception>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>
#include <opencv2/opencv.hpp>

class LibException : public std::exception {
	std::string message;

public:
	LibException(const std::string _message) :
		message(_message) {
	}

	const char * what() const throw () {
		return message.c_str();
	}
};

class Module {
protected:
	boost::signals2::signal<void(const cv::Mat&, long long)> pc_signal;

public:
	virtual void operator()(const cv::Mat&, long long timestamp) = 0;

	void Connect(Module& module) {
		pc_signal.connect(bind(&Module::operator(), &module, _1, _2));
	}
};

