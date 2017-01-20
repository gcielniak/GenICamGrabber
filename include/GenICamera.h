#pragma once
#include <boost/thread.hpp>
#include <iCVCDriver.h>
#include <iCVCUtilities.h>
#include <iCVGenApi.h>
#include <CVCError.h>
#include "Module.h"

using namespace std;

class GenICamera : public Module {
protected:
	bool is_running = false;
	bool initialised = false;
	IMG hCamera = NULL;
	boost::thread thread;

	int ocv_depth(cvbdatatype_t cvbDt) {
		// map compatible cvb data type descriptors to OpenCV data types 
		switch (cvbDt & 0xFF) {
		case 8: return IsSignedDatatype(cvbDt) ? CV_8S : CV_8U;
		case 16: return IsSignedDatatype(cvbDt) ? CV_16S : CV_16U;
		case 32: return IsFloatDatatype(cvbDt) ? CV_32F : CV_32S;
		case 64: return IsFloatDatatype(cvbDt) ? CV_64F : 0;
		default: return 0;
		}
	}

	void Capture() {
		//start grabbing

		int type;

		if (ImageDimension(hCamera) == 3)
			type = CV_8UC3;
		else
			type = CV_8U;
		cv::Mat image(cv::Size(ImageWidth(hCamera), ImageHeight(hCamera)), type);

		if (type == CV_8UC3) {
			cv::Mat image2(cv::Size(ImageWidth(hCamera), ImageHeight(hCamera)), type);
			cv::cvtColor(image, image2, CV_BGR2RGB);
			image = image2;
		}
		double t_start = 0.0, t_now;
		while (is_running) {
			if (G2Wait(hCamera) >= 0) {
				//assign newly captured buffer to OpenCV Mat object
				void* ppixels = nullptr; intptr_t xInc = 0; intptr_t yInc = 0;
				GetLinearAccess(hCamera, 0, &ppixels, &xInc, &yInc);
				image.data = (uchar*)ppixels;
				//calculate timestamp in nanoseconds
				G2GetGrabStatus(hCamera, GRAB_INFO_CMD::GRAB_INFO_TIMESTAMP, t_now);
				if (t_start == 0.0)
					t_start = t_now;
				long long timestamp = (long long)(t_now - t_start);
				pc_signal(image, timestamp);
			}
		}
	}

public:
	GenICamera() {
	}

	~GenICamera() {
		Stop();
		ReleaseObject(hCamera);
	}

	virtual void operator()(const cv::Mat& image, long long timestamp) {
	}

	void Init() {
		if (!initialised) {
			static const size_t DRIVERPATHSIZE = 256;
			// load the first camera
			char driverPath[DRIVERPATHSIZE] = { 0 };
			TranslateFileName("%CVB%\\Drivers\\GenICam.vin", driverPath, DRIVERPATHSIZE);
			if (!LoadImageFile(driverPath, hCamera))
				throw LibException("GenICamera::Init, Error loading \"" + std::string(driverPath) + "\" driver!");
			initialised = true;
		}
	}

	void NodeList() {
		char name[256] = { 0 };
		size_t name_size = sizeof(name);
		NODEMAP hNodeMap;
		NMHGetNodeMap(hCamera, hNodeMap);
		cvbdim_t count;
		NMNodeCount(hNodeMap, count);
		NMListNode(hNodeMap, 1, name, name_size);
		cerr << count << endl;
		cerr << name << endl;
	}

	void NodeInfo(NODE node) {
		TNodeType node_type;
		char name[256] = { 0 };
		size_t name_length = sizeof(name);

		NType(node, node_type);

		if (node_type == NT_EnumEntry)
			return;

		NInfoAsString(node, ::TNodeInfo::NI_Name, name, name_length);
		cerr << name << ": ";

		NInfoAsString(node, ::TNodeInfo::NI_DisplayName, name, name_length);
		cerr << name << ": ";

		cvbbool_t bvalue;
		double fvalue;
		cvbint64_t ivalue;

		switch (node_type) {
		case NT_Boolean:
			NGetAsBoolean(node, bvalue);
			cerr << bvalue << endl;
			break;
		case NT_Command:
			cerr << "command" << endl;
			break;
		case NT_Category:
			cerr << "category" << endl;
			break;
		case NT_Float:
			NGetAsFloat(node, fvalue);
			cerr << fvalue << endl;
			break;
		case NT_Integer:
			NGetAsInteger(node, ivalue);
			cerr << ivalue << endl;
			break;
		case NT_String:
			NGetAsString(node, name, name_length);
			cerr << name << endl;
			break;
		case NT_EnumEntry:
			//ignore as these will appear in Enumeration
			break;
		case NT_Enumeration:
			cvbint64_t num_entries;
			NInfoAsInteger(node, ::TNodeInfo::NI_NumEnumEntries, num_entries);
			for (cvbdim_t i = 0; i < num_entries; i++) {
				NInfoEnum(node, i, ivalue, name, name_length);
				cerr << name << " (" << ivalue << "), ";
			}
			cerr << endl;
			break;
		default:
			cerr << "value type " << node_type << endl;
			break;
		}
	}

	void genicam_access() {
		cerr << "----------------" << endl;
		NodeList();
		cerr << "----------------" << endl;
		NODEMAP hNodeMap = NULL;
		cvbres_t result = NMHGetNodeMap(hCamera, hNodeMap);
		cvbval_t value;
		NMNodeCount(hNodeMap, value);
		cerr << "NMNodeCount: " << value << endl;

		char name[256] = { 0 };
		size_t name_length = sizeof(name);

		for (int i = 0; i < value; i++) {
			NMListNode(hNodeMap, i, name, name_length);
			NODE node;
			NMGetNode(hNodeMap, name, node);
			NodeInfo(node);
		}
	}

	int GetNrDevices() {
		cvbval_t value = 0;

		if (CanCameraSelect2(hCamera))
			CS2GetNumPorts(hCamera, value);

		return value;
	}

	void SetCamera(int index) {
		if (CanCameraSelect2(hCamera))
			CS2SetCamPort(hCamera, index, 0, hCamera);
	}

	int GetCameraIndex() {
		cvbval_t port;
		CS2GetCamPort(hCamera, port);
		return port;
	}

	string GetNodeValue(string node_name) {
		NODE node;
		NODEMAP node_map;
		TNodeType node_type;
		char name[256] = { 0 };
		size_t name_size = sizeof(name);

		NMHGetNodeMap(hCamera, node_map);
		NMGetNode(node_map, node_name.c_str(), node);
		NType(node, node_type);
		if (node_type == NT_String)
			NGetAsString(node, name, name_size);
		ReleaseObject(node);
		ReleaseObject(node_map);

		return name;
	}

	string GetDeviceName(int index) {
		stringstream device_name;

		if (index < GetNrDevices()) {
			device_name << GetNodeValue("DeviceFamilyName" );
			device_name << " " << GetNodeValue("DeviceModelName");
			device_name << " by " << GetNodeValue("DeviceVendorName");
		}

		return device_name.str();
	}

	void Start() {
		Init();
		if (G2Grab(hCamera) < 0) {
			throw LibException("GenICamera::Capture, G2Grab function failure.");
			return;
		}
		thread = boost::thread(&GenICamera::Capture, this);
		is_running = true;
	}

	void Stop() {
		is_running = false;
		thread.join();
		G2Freeze(hCamera, true);
	}

	bool IsRunning() {
		return is_running;
	}
};
