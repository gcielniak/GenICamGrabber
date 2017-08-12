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

class CameraManager {
	vector<string> present_cameras;

public:
	static void GetAvailableDevices(std::vector<string>& camera_list) {
#ifdef HAVE_GENICAM
		//GenICam devices
		try {
			GenICam::Camera camera;
			camera.Init();
			camera.GetDevices(camera_list);

			//JAI
			if ((find(camera_list.begin(), camera_list.end(), "AD-130GE_#0") != camera_list.end()) && 
				(find(camera_list.begin(), camera_list.end(), "AD-130GE_#1") != camera_list.end())) {
				camera_list.insert(camera_list.begin(), "AD-130GE RGBN");
			}

			//Teledyne DALSA
			if ((find(camera_list.begin(), camera_list.end(), "Nano-C2420") != camera_list.end()) &&
				(find(camera_list.begin(), camera_list.end(), "Nano-M2420") != camera_list.end())) {
				camera_list.insert(camera_list.begin(), "Nano RGBN");
			}
		}
		catch (std::runtime_error&) {
		}
#endif
	}

	static InputDeviceBase* GetCamera(int plaform = 0, int device = 0) {
		InputDeviceBase* camera = 0;
		switch (device) {
#ifdef HAVE_GENICAM
		case 0:
			camera = new InputDevice<JAI::AD_130GE>();
			camera->Init();
			break;
		case 1:
			camera = new InputDevice<GenICam::Camera>();
			camera->Init();
			break;
		case 2:
			camera = new InputDevice<GenICam::Camera>();
			camera->Init(1);
			break;
#endif
		default:
			throw std::runtime_error("CameraManager::GetCamera, no cameras present.");
		}
		return camera;
	}
};
