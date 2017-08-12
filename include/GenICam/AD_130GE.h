#pragma once

#include "Camera.h"
#include "GenICam/Camera.h"

namespace JAI {
	class AD_130GE : public RGBNCamera<GenICam::Camera> {
	protected:
		bool software_trigger;

	public:
		AD_130GE() : software_trigger(false) {
		}

		virtual void Init(int device_nr = 0) {
			try {
				RGBNCamera<GenICam::Camera>::Init();
			}
			catch (std::runtime_error&) {
				throw std::runtime_error("Could not initialise the AD-130GE camera.");
			}

			//assign the right port
			if (camera_rgb.Name() == "AD-130GE_#1")
				camera_rgb.Port(1);
			else if (camera_nir.Name() == "AD-130GE_#0")
				camera_nir.Port(1);

			if ((camera_rgb.Name() != "AD-130GE_#0") || (camera_nir.Name() != "AD-130GE_#1"))
				throw std::runtime_error("Could not initialise the AD-130GE camera.");

			if ((camera_rgb.ImageType() != CV_8UC3) && (camera_nir.ImageType() != CV_8UC1))
				throw std::runtime_error("AD-130GE camera found but with wrong image format.");

			//a fix that sets the rgb image to 966
			//requires another initialisation which needs to be investigated
			camera_rgb.SetValue("Height", 966);
			camera_rgb.Init();
			if (camera_rgb.Name() == "AD-130GE_#1")
				camera_rgb.Port(1);

			//enforce two camera sync
			camera_rgb.SetValue("SyncMode", 0);

			//adjust auto-tune parameters
			camera_rgb.SetValue("GainAuto", 1);
			camera_nir.SetValue("GainAuto", 1);

			SoftwareTrigger(true);

			cerr << "AD-130GE camera found." << endl;
		}

		void SoftwareTrigger(bool value) {
			software_trigger = value;
			camera_rgb.TriggerMode(value);
			camera_nir.TriggerMode(value);
			if (software_trigger) {
				camera_rgb.SetValue("TriggerSource", 64);
				camera_nir.SetValue("TriggerSource", 64);
			}
		}

		virtual void Capture() {
			if (software_trigger)
				camera_rgb.TriggerSoftware();

			::RGBNCamera<GenICam::Camera>::Capture();
		}

		virtual const std::string Name() {
			return "AD-130GE RGBN";
		}
	};
}
