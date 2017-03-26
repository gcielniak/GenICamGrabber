
#include <iCVCDriver.h>
#include <iCVCUtilities.h>
#include <iCVGenApi.h>
#include <CVCError.h>

using namespace std;

class GenICamDevice {
protected:
	bool initialised = false;
	IMG hCamera = NULL;
	NODEMAP node_map = NULL;
	double t_start = 0.0;

public:
	GenICamDevice() {
	}

	~GenICamDevice() {
		ReleaseObject(node_map);
		ReleaseObject(hCamera);
	}

	void Init() {
		if (!initialised) {
			static const size_t DRIVERPATHSIZE = 256;
			// load the first camera
			char driverPath[DRIVERPATHSIZE] = { 0 };
			TranslateFileName("%CVB%\\Drivers\\GenICam.vin", driverPath, DRIVERPATHSIZE);
			if (!LoadImageFile(driverPath, hCamera)) {
				string message = "GenICamera::Init, Error loading \"" + std::string(driverPath) + "\" driver!";
				throw std::exception(message.c_str());
			}
			NMHGetNodeMap(hCamera, node_map);
			initialised = true;
		}
	}

	void GetImageProps(int& width, int& height, int&channels, int&pixel_depth) {
		width = ImageWidth(hCamera);
		height = ImageHeight(hCamera);
		channels = ImageDimension(hCamera);
		pixel_depth = BitsPerPixel(ImageDatatype(hCamera, 0));
	}

	unsigned char* GetBuffer() {
		//wait for a new buffer
		while (G2Wait(hCamera) < 0);

		void* ppixels = nullptr; intptr_t xInc = 0; intptr_t yInc = 0;
		GetLinearAccess(hCamera, 0, &ppixels, &xInc, &yInc);
		return (unsigned char*)ppixels;
	}

	void Start() {
		Init();
		if (G2Grab(hCamera) < 0) {
			throw LibException("GenICamera::Capture, G2Grab function failure.");
			return;
		}
	}

	void Stop() {
		initialised = false;
		G2Freeze(hCamera, true);
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
		if (CanCameraSelect2(hCamera)) {
			CS2SetCamPort(hCamera, index, 0, hCamera);
			NMHGetNodeMap(hCamera, node_map);
		}
	}

	int GetCameraIndex() {
		cvbval_t port;
		CS2GetCamPort(hCamera, port);
		return port;
	}

	string GetNodeValue(string node_name) {
		NODE node;
		char name[256] = { 0 };
		size_t name_size = sizeof(name);

		NMGetNode(node_map, node_name.c_str(), node);
		NGetAsString(node, name, name_size);
		ReleaseObject(node);

		return name;
	}

	double GetNodeValueDouble(string node_name) {
		NODE node;
		double value;

		NMGetNode(node_map, node_name.c_str(), node);
		NGetAsFloat(node, value);
		ReleaseObject(node);

		return value;
	}

	bool GetNodeValueBoolean(string node_name) {
		NODE node;
		cvbbool_t value;

		NMGetNode(node_map, node_name.c_str(), node);
		NGetAsBoolean(node, value);
		ReleaseObject(node);

		return value ? true : false;
	}

	int GetNodeValueInteger(string node_name) {
		NODE node;
		cvbint64_t value;

		NMGetNode(node_map, node_name.c_str(), node);
		NGetAsInteger(node, value);

		char name[256] = { 0 };
		size_t name_size = sizeof(name);
		NInfoAsString(node, TNodeInfo::NI_AccessMode, name, name_size);
		cerr << node_name << ", accessMode: " << name << endl;

		ReleaseObject(node);

		return (int)value;
	}

	void SetNodeValue(string node_name, string value) {
		NODE node;

		NMGetNode(node_map, node_name.c_str(), node);
		NSetAsString(node, value.c_str());
		ReleaseObject(node);
	}

	void SetNodeValue(string node_name, double value) {
		NODE node;

		NMGetNode(node_map, node_name.c_str(), node);
		NSetAsFloat(node, value);
		ReleaseObject(node);
	}

	void SetNodeValue(string node_name, int value) {
		NODE node;

		NMGetNode(node_map, node_name.c_str(), node);
		NSetAsInteger(node, value);
		ReleaseObject(node);
	}

	void SetNodeValue(string node_name, bool value) {
		NODE node;

		NMGetNode(node_map, node_name.c_str(), node);
		NSetAsBoolean(node, value);
		ReleaseObject(node);
	}

	string GetDeviceName(int index) {
		stringstream device_name;

		if (index < GetNrDevices()) {
			device_name << GetNodeValue("DeviceFamilyName");
			device_name << " " << GetNodeValue("DeviceModelName");
			device_name << " by " << GetNodeValue("DeviceVendorName");
		}

		return device_name.str();
	}

	void Gain(double value) { SetNodeValue("Gain", value); }
	double Gain() { return GetNodeValueDouble("Gain"); }

	void ExposureTime(double value) { SetNodeValue("ExposureTime", value); }
	double ExposureTime() { return GetNodeValueDouble("ExposureTime"); }

	void AutoBrightness(bool value) { SetNodeValue("autoBrightnessMode", value ? 1 : 0); }
	bool AutoBrightness() { return (GetNodeValue("autoBrightnessMode") == "Active") ? true : false; }

	void BalanceRatioRed(double value) {
		SetNodeValue("BalanceRatioSelector", "Red");
		SetNodeValue("BalanceRatio", value);
	}

	double BalanceRatioRed() {
		SetNodeValue("BalanceRatioSelector", "Red");
		return GetNodeValueDouble("BalanceRatio");
	}

	void BalanceRatioGreen(double value) {
		SetNodeValue("BalanceRatioSelector", "Green");
		SetNodeValue("BalanceRatio", value);
	}

	double BalanceRatioGreen() {
		SetNodeValue("BalanceRatioSelector", "Green");
		return GetNodeValueDouble("BalanceRatio");
	}

	void BalanceRatioBlue(double value) {
		SetNodeValue("BalanceRatioSelector", "Blue");
		SetNodeValue("BalanceRatio", value);
	}

	double BalanceRatioBlue() {
		SetNodeValue("BalanceRatioSelector", "Blue");
		return GetNodeValueDouble("BalanceRatio");
	}

	void TriggerMode(bool value) { SetNodeValue("TriggerMode", value ? 1 : 0); }
	bool TriggerMode() { return (GetNodeValueInteger("TriggerMode") == 1) ? true : false; }

	void TriggerSoftware() { SetNodeValue("TriggerSoftware", true); }

	long long GetTimeStamp() {
		//calculate timestamp in nanoseconds
		double t_now;
		G2GetGrabStatus(hCamera, GRAB_INFO_CMD::GRAB_INFO_TIMESTAMP, t_now);
		if (t_start == 0.0)
			t_start = t_now;
		long long timestamp = (long long)(t_now - t_start);
		return timestamp;
	}
};
