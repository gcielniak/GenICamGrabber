#pragma once

#include "GenICam/Parameter.h"

///
/// A GenICam Camera class.
///

namespace GenICam {
	class Camera : public ::Camera, public ::GenICam::Parameter {
	private:
		double t_start = 0.0, t_now;

	protected:
		IMG cvb_image;

		static IMG LoadDriver(std::string driver_name = "%CVB%\\Drivers\\GenICam.vin") {
			static const size_t DRIVERPATHSIZE = 256;
			IMG image;

			char driverPath[DRIVERPATHSIZE] = { 0 };
			cvbbool_t success = TranslateFileName(driver_name.c_str(), driverPath, DRIVERPATHSIZE);
			if (!success)
				throw std::runtime_error("GenICam::Init, TranslateFileName failed.");

			success = LoadImageFile(driverPath, image);
			if (!success) {
				string message = "GenICam::Init, Error loading \"" + std::string(driverPath) + "\" driver!";
				throw std::runtime_error(message.c_str());
			}
			return image;
		}

	public:
		Camera() {
		}

		~Camera() {
			ReleaseObject(cvb_image);
		}

		virtual void Init(int device_nr = 0) {
			cvb_image = LoadDriver();
			Update(cvb_image);
			Port(device_nr);
			TriggerMode(false);
		}

		virtual void Start() {
			if (G2Grab(cvb_image) < 0)
				throw std::runtime_error("Camera::Start, G2Grab failed.");

			if (ImageType() == CV_8UC3)
				image = new RGBImage(cv::Mat(cv::Size(ImageWidth(cvb_image), ImageHeight(cvb_image)), ImageType()), 0);
			else
				image = new MonoImage(cv::Mat(cv::Size(ImageWidth(cvb_image), ImageHeight(cvb_image)), ImageType()), 0);
		}

		virtual void Stop() {
			if (G2Freeze(cvb_image, true) < 0)
				throw std::runtime_error("Camera::Stop, G2Freeze failed.");
		}

		virtual void Capture() {
			while (G2Wait(cvb_image) < 0);//TODO: introduce time_out
			void* ppixels = nullptr; intptr_t xInc = 0; intptr_t yInc = 0;
			GetLinearAccess(cvb_image, 0, &ppixels, &xInc, &yInc);
			image->image.data = (uchar*)ppixels;
			//swap colour channels so it corresponds to OpenCV BGR format
			if (image->image.type() == CV_8UC3)
				cv::cvtColor(image->image, image->image, CV_RGB2BGR);
			image->timestamp = GetTimeStamp();
		}

		virtual long long GetTimeStamp() {
			//calculate timestamp in nanoseconds
			G2GetGrabStatus(cvb_image, GRAB_INFO_CMD::GRAB_INFO_TIMESTAMP, t_now);
			if (t_start == 0.0)
				t_start = t_now;
			long long timestamp = (long long)(t_now - t_start);
			return timestamp;
		}

		virtual const std::string Name() {
			return GetValueString("DeviceModelName");
		}

		void Port(int index) {
			cvbbool_t success = CanCameraSelect2(cvb_image);
			if (!success)
				throw std::runtime_error("Camera::SetPort, CanCameraSelect2 failed.");

			cvbres_t error = CS2SetCamPort(cvb_image, index, 0, cvb_image);
			if (error < 0)
				throw std::runtime_error("Camera::SetPort, CS2SetCamPort failed.");

			Update(cvb_image);
		}

		int Port() {
			cvbval_t port;
			cvbres_t error = CS2GetCamPort(cvb_image, port);
			if (error < 0)
				throw std::runtime_error("Camera::GetPort, CS2GetCamPort failed.");

			return port;
		}

		int PortNr() {
			cvbbool_t success = CanCameraSelect2(cvb_image);
			if (!success)
				throw std::runtime_error("Camera::PortNr, CanCameraSelect2 failed.");

			cvbval_t value = 0;

			cvbres_t error = CS2GetNumPorts(cvb_image, value);
			if (error < 0)
				throw std::runtime_error("Camera::PortNr, CS2GetNumPorts failed.");

			return value;
		}

		int ImageType() {
			int type;
			if ((ImageDimension(cvb_image) == 3) && BitsPerPixel(ImageDatatype(cvb_image, 0) == 8))
				type = CV_8UC3;
			else if ((ImageDimension(cvb_image) == 1) && BitsPerPixel(ImageDatatype(cvb_image, 0) == 8))
				type = CV_8UC1;
			else if ((ImageDimension(cvb_image) == 1) && BitsPerPixel(ImageDatatype(cvb_image, 0) == 16))
				type = CV_16UC1;
			return type;
		}

		void GetDevices(vector<string>& names) {
			int camera_index = 0;
			while (true) {
				try {
					Port(camera_index++);
					names.push_back(Name());
				}
				catch (std::runtime_error&) {
					return;
				}
			}
		}
	};
}
