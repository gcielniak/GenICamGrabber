#pragma once

#include "Image.h"

///
/// A generic Camera class.
///

class Camera {
protected:
	Image *image;

public:
	Camera() : image(0) {
	}

	virtual void Init(int device_nr) = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Capture() = 0;
	virtual long long GetTimeStamp() = 0;
	virtual const std::string Name() = 0;
	const Image* GetImage() {
		return image;
	}
};

///
/// Dual camera setup with RGB and NIR sensor
///

template < typename CameraT>
class RGBNCamera : public Camera {
protected:
	CameraT camera_rgb, camera_nir;

public:
	RGBNCamera() {
		image = new RGBNImage();
	}

	virtual void Init(int device_nr = 0) {
		camera_rgb.Init();
		camera_nir.Init();
	}

	virtual void Start() {
		camera_rgb.Start();
		camera_nir.Start();
	}

	virtual void Stop() {
		camera_rgb.Stop();
		camera_nir.Stop();
	}

	virtual void Capture() {
		camera_rgb.Capture();
		camera_nir.Capture();
		std::vector<cv::Mat> channels;
		cv::split(camera_rgb.GetImage()->image, channels); // break image into channels
		channels.push_back(camera_nir.GetImage()->image);
		cv::merge(channels, image->image); // combine rgb and nir channels
		image->timestamp = GetTimeStamp();
	}

	virtual long long GetTimeStamp() {
		return camera_rgb.GetTimeStamp();
	}

	const Image* GetRGBImage() {
		return camera_rgb.GetImage();
	}

	const Image* GetNIRImage() {
		return camera_nir.GetImage();
	}
};
