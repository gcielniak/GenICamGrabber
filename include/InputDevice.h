#pragma once

#include <string>
#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include "SignalConnector.h"
#include "Camera.h"

class InputDeviceBase : public SignalConnector<const Image&> {
protected:
	virtual void Capture() = 0;

public:
	virtual void Init(int device_nr = 0) = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
};


template <typename CameraT>
class InputDevice : public InputDeviceBase {
	bool is_running = false;
	bool initialised = false;
	boost::thread thread;

protected:
	virtual void Capture() {
		//start grabbing
		while (is_running) {
			camera.Capture();
			signal(*camera.GetImage());
		}
	}

public:
	CameraT camera;

	virtual void Init(int device_nr) {
		camera.Init(device_nr);
		initialised = true;
	}

	virtual void Start() {
		if (!initialised)
			Init(0);

		if (!is_running) {
			camera.Start();
			is_running = true;
			thread = boost::thread(&InputDevice::Capture, this);
		}
	}

	virtual void Stop() {
		is_running = false;
		initialised = false;
		thread.join();
	}

	virtual bool IsRunning() {
		return is_running;
	}
};

