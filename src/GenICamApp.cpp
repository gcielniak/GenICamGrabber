#include "CameraManager.h"
#include "ImageViewer.h"
#include "ImageWriter.h"
#include "AutoFocus.h"
#include "RGBN2NDVI.h"
#include "HistogramViewer.h"

using namespace std;

string Help() {
	stringstream sstream;

	sstream << "CameraGrabber usage:" << endl;
	sstream << "  -l : list all available input platforms and devices" << endl;
	sstream << "  -p : specify the input platform" << endl;
	sstream << "  -d : specify the input device" << endl;
	sstream << "  -h : print this message" << endl;

	return sstream.str();
}

int main(int argc, char* argv[]) {

	int platform = 0, device = 0;

	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-p") == 0) && (i < (argc - 1))) { platform = atoi(argv[++i]); }
		else if ((strcmp(argv[i], "-d") == 0) && (i < (argc - 1))) { device = atoi(argv[++i]); }
		else if (strcmp(argv[i], "-l") == 0) {
			cerr << CameraManager::ListAllDevices() << endl;
			return 0;
		}
		else if (strcmp(argv[i], "-h") == 0) {
			cerr << Help() << endl; 
			return 0;
		}
	}

	InputDeviceBase* camera;

	try {
		camera = CameraManager::GetCamera(platform, device);
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
//	camera->Connect(hist_viewer);
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
