#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Camera.h"
#include "InputDevice.h"
#ifdef HAVE_GENICAM
#include "GenICam/AD_130GE.h"
#include "GenICam/GenieNano.h"
#endif

using namespace std;

struct CameraManager {
	enum PlatformType {
		GENICAM_PLATFORM
	};

	static void GetSupportedPlatforms(std::vector<PlatformType>& supported_platforms) {
#ifdef HAVE_GENICAM
		supported_platforms.push_back(GENICAM_PLATFORM);
#endif
	}

	static void GetAvailablePlatforms(std::vector<PlatformType>& available_platforms) {
		std::vector<PlatformType> platforms;
		GetSupportedPlatforms(platforms);

		for (auto platform : platforms) {
			std::vector<string> devices;
			GetAvailableDevices(platform, devices);
			if (devices.size())
				available_platforms.push_back(platform);
		}
	}

	static void GetPlatformNames(const std::vector<PlatformType>& platforms, std::vector<std::string>& names) {
		for (auto platform : platforms) {
			switch (platform) {
			case GENICAM_PLATFORM:
				names.push_back("GenICam");
				break;
			}
		}
	}

	static void GetAvailableDevices(PlatformType type, std::vector<string>& devices) {
		switch (type) {
#ifdef HAVE_GENICAM
		case GENICAM_PLATFORM:
			//GenICam devices
			GenICam::Camera camera;
			try {
				camera.Init();
				camera.GetDevices(devices);
			}
			catch (std::exception&) {
			}
			//JAI
			if ((find(devices.begin(), devices.end(), "AD-130GE_#0") != devices.end()) &&
				(find(devices.begin(), devices.end(), "AD-130GE_#1") != devices.end())) {
				devices.insert(devices.begin(), "AD-130GE RGBN");
			}

			//Teledyne DALSA
			if ((find(devices.begin(), devices.end(), "Nano-C2420") != devices.end()) &&
				(find(devices.begin(), devices.end(), "Nano-M2420") != devices.end())) {
				devices.insert(devices.begin(), "Nano RGBN");
			}
			break;
#endif
		}
	}

	static string ListAllDevices() {
		stringstream sstream;
		std::vector<PlatformType> supported_platforms, available_platforms;
		std::vector<string> platform_names;
		
		GetSupportedPlatforms(supported_platforms);
		GetAvailablePlatforms(available_platforms);
		GetPlatformNames(available_platforms, platform_names);

		//Supported platforms present in OS
		sstream << "Supported platforms:";
		for (auto platform : supported_platforms) {
			switch (platform) {
			case GENICAM_PLATFORM:
				sstream << " GenICam";
				break;
			}
			if (platform != supported_platforms.back())
				sstream << ",";
			else
				sstream << endl;
		}

		//Available platforms: with at least one active device
		sstream << "Available platforms:";
		for (auto platform : available_platforms) {
			switch (platform) {
			case GENICAM_PLATFORM:
				sstream << " GenICam";
				break;
			}
			if (platform != available_platforms.back())
				sstream << ",";
			else
				sstream << endl;
		}

		//List all platforms and devices
		sstream << "Available platforms and devices:" << endl;
		for (int i = 0; i < platform_names.size(); i++) {
			sstream << " Platform " << i << ": " << platform_names[i] << endl;
			std::vector<string> devices;
			GetAvailableDevices(available_platforms[i], devices);
			for (int j = 0; j < devices.size(); j++)
				sstream << "  Device " << j << ": " << devices[j] << endl;
		}

		return sstream.str();
	}

	static InputDeviceBase* GetCamera(int platform_index=0, int device_index=0) {
		std::vector<PlatformType> platforms;
		GetAvailablePlatforms(platforms);
		if (platforms.size() <= platform_index)
			throw std::runtime_error("CameraManager::GetCamera, wrong platform index.");

		InputDeviceBase* camera = 0;
		std::vector<string> devices;

		switch (platforms[platform_index]) {
#ifdef HAVE_GENICAM
		case GENICAM_PLATFORM:
			GetAvailableDevices(GENICAM_PLATFORM, devices);
			if (devices.size() <= device_index)
				throw std::runtime_error("CameraManager::GetCamera, wrong device index.");

			if (devices[device_index] == "AD-130GE RGBN") {
				camera = new InputDevice<JAI::AD_130GE>();
				camera->Init();
				return camera;
			}
			else if (devices[device_index] == "Nano RGBN") {
				camera = new InputDevice<TeledyneDALSA::GenieNano>();
				camera->Init();
				return camera;
			}
			else {
				camera = new InputDevice<GenICam::Camera>();
				//if there are RGBN cameras in the list one needs to increment the device_index
				if (device_index && (devices[0] == "AD-130GE RGBN" || devices[0] == "Nano RGBN"))
					camera->Init(device_index - 1);
				else
					camera->Init(device_index);
				return camera;
			}
			break;
#endif
		}

		throw std::runtime_error("CameraManager::GetCamera, no cameras present.");
	}
};
