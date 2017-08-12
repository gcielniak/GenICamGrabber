#include "CameraManager.h"
#include "ImageViewer.h"
#include "ImageWriter.h"
#include "AutoFocus.h"
#include "RGBN2NDVI.h"
#include "HistogramViewer.h"

using namespace std;

int main(int argc, char* argv[]) {

	vector<string> devices;

	CameraManager::GetAvailableDevices(devices);

	for (int i = 0; i < devices.size(); i++)
		cerr << "Device " << i << ": " << devices[i] << endl;

	int device_nr = 0;

	InputDeviceBase* camera = 0;

	try {
		camera = CameraManager::GetCamera(0, device_nr);
	}
	catch (std::exception& e) {
		cerr << e.what() << endl;
		return 0;
	}

	RGBN2NDVI ndvi_converter;

	ImageViewer viewer("RGBN"), viewer_ndvi("NDVI");
	HistogramViewer hist_viewer("HIST");

//	camera.Connect(ndvi_converter);
	camera->Connect(viewer);
	camera->Connect(hist_viewer);
//	ndvi_converter.Connect(viewer_ndvi);

	try {

		camera->Start();

		while (viewer.IsRunning());

		camera->Stop();
	}
	catch (std::exception& e)	{
		cerr << e.what() << endl;
	}

	return 0;
}
