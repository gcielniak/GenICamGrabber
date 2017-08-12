#pragma once
#include <iostream>
#include <boost/thread.hpp>
#include "Grabber.h"
#include "GenICam/AD_130GE.h"
#include "GenICam/GenieNano.h"

class GenICamGrabber : public Grabber {
protected:
	GenICam::Camera camera;
	void Capture() {
		//start grabbing
		while (is_running) {
			image_signal(*camera.Capture());
		}
	}

public:
	virtual void operator()(const Image&) {
	}

	void Init() {
		camera.Init();
		camera.TriggerMode(false);
		initialised = true;
	}

	void Start() {
		if (!initialised)
			Init();

		if (!is_running) {
			camera.Start();
			is_running = true;
			thread = boost::thread(&GenICamGrabber::Capture, this);
		}
	}

};

class RGBNGrabber : public Grabber {
protected:
	int type;
	GenICam::RGBNCamera* camera;

	float dx, dy;

	void Capture() {
		//start grabbing
		while (is_running) {
			camera->Capture();
			rgb_signal(*camera->RGBImage());
			nir_signal(*camera->NIRImage());
			rgbn_signal(*camera->RGBNImage());
		}
	}

public:
	boost::signals2::signal<void(const Image&)> rgb_signal;
	boost::signals2::signal<void(const Image&)> nir_signal;
	boost::signals2::signal<void(const Image&)> rgbn_signal;

	void keyboard_handler(int key) {
		switch (key) {
		case 119:
			dy += 1.0;
			break;
		case 115:
			dy -= 1.0;
			break;
		case 97:
			dx += 1.0;
			break;
		case 100:
			dx -= 1.0;
			break;
		default:
			return;
		}
		cerr << dx << " " << dy << endl;
//		camera.SetTrans(dx, dy);
	}

	RGBNGrabber() {
		dx = 37.0;
		dy = -47.0;
//		camera.SetTrans(dx, dy);
	}

	void AutoDetect() {
		GenICam::Camera cam;
		cam.Init();
		string name = cam.Name();
		if ((name == "AD-130GE_#0") || (name == "AD-130GE_#1"))
			camera = new JAI::AD_130GE();
		else if ((name == "Nano-C2420") || (name == "Nano-M2420"))
			camera = new TeledyneDALSA::GenieNano();
		else
			throw std::runtime_error("Could not autodetect the RGBN camera.");
	}

	virtual void operator()(const Image&) {
	}

	void Init() {
		AutoDetect();
		camera->Init();
		initialised = true;
	}

	void Start() {
		if (!initialised)
			Init();

		if (!is_running) {
			camera->Start();
			is_running = true;
			thread = boost::thread(&RGBNGrabber::Capture, this);
		}
	}
};
