#pragma once

#include "GenICam/Camera.h"

namespace TeledyneDALSA {
	class GenieNano : public ::RGBNCamera<GenICam::Camera> {
		float dx, dy;
		bool software_trigger;

	public:
		GenieNano() : dx(0.0), dy(0.0), software_trigger(false) {
		}

		void Init(int device_nr = 0) {
			try {
				RGBNCamera<GenICam::Camera>::Init();
			}
			catch (std::runtime_error&) {
				throw std::runtime_error("Could not initialise the Genie Nano camera.");
			}

			//assign the right port
			if (camera_rgb.Name() == "Nano-M2420")
				camera_rgb.Port(1);
			else if (camera_nir.Name() == "Nano-C2420")
				camera_nir.Port(1);

			if ((camera_rgb.Name() != "Nano-C2420") || (camera_nir.Name() != "Nano-M2420"))
				throw std::runtime_error("Could not initialise the Genie Nano camera.");
			if ((camera_rgb.ImageType() != CV_8UC3) && (camera_nir.ImageType() != CV_8UC1))
				throw std::runtime_error("Genie Nano camera found but with wrong image format.");

			camera_rgb.AutoBrightness(true);
			camera_nir.AutoBrightness(true);
		}

		virtual void SoftwareTrigger(bool value) {
			software_trigger = value;
			if (software_trigger) {
				camera_rgb.TriggerMode(true);
				camera_nir.TriggerMode(true);
				camera_rgb.SetValue("LineSelector", 2);
				camera_rgb.SetValue("outputLineSource", 5);
			}
		}

		//capture RGBNIR image stored as 4D matrix
		virtual void Capture() {
			if (software_trigger)
				camera_rgb.TriggerSoftware();

			::RGBNCamera<GenICam::Camera>::Capture();

			std::vector<cv::Mat> channels;
			cv::split(camera_rgb.GetImage()->image, channels); // break image into channels

			cv::Mat M = cv::Mat::eye(2, 3, CV_32F);
			M.at<float>(0, 2) = dx;
			M.at<float>(1, 2) = dy;

			cv::warpAffine(camera_rgb.GetImage()->image, camera_rgb.GetImage()->image, M, camera_rgb.GetImage()->image.size());
			channels.push_back(camera_rgb.GetImage()->image);
			cv::merge(channels, camera_rgb.GetImage()->image); // combine channels
		}

		virtual const std::string Name() {
			return "Nano RGBN";
		}
		
		void SetTrans(float dx_new, float dy_new) {
			dx = dx_new;
			dy = dy_new;
		}
	};
}
