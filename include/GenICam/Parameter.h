#pragma once

#include <string>
#include <iostream>

#include <iCVCDriver.h>
#include <iCVGenApi.h>
#include <iCVCUtilities.h>

using namespace std;

namespace GenICam {

	class ParameterInterface {
		NODEMAP node_map;

	protected:
		//on LoadDriver and SetCamera
		void Update(IMG cvb_image) {
			if (NMHGetNodeMap(cvb_image, node_map) < 0)
				throw std::runtime_error("Node::UpdateNodeMap, NMHGetNodeMap failed.");
		}

		NODE GetNode(const string& name) {
			NODE node;
			if (NMGetNode(node_map, name.c_str(), node) < 0)
				throw std::runtime_error("Node::GetNode, NMGetNode failed.");
			return node;
		}
	public:
		ParameterInterface() : node_map(0) {
		}

		~ParameterInterface() {
			if (node_map)
				ReleaseObject(node_map);
		}

		void SetValue(const string& node_name, int value) {
			if (NSetAsInteger(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::SetValue, NSetAsInteger failed.");
		}

		void SetValue(const string& node_name, double value) {
			if (NSetAsFloat(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::SetValue, NSetAsFloat failed.");
		}

		void SetValue(const string& node_name, bool value) {
			if (NSetAsBoolean(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::SetValue, NSetAsBoolean failed.");
		}

		void SetValue(const string& node_name, const string& value) {
			if (NSetAsString(GetNode(node_name), value.c_str()) < 0)
				throw std::runtime_error("Node::SetValue, NSetAsString failed.");
		}

		int GetValueInteger(const string& node_name) {
			cvbint64_t value;

			if (NGetAsInteger(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::GetValueInteger, NGetAsInteger failed.");

			return (int)value;
		}

		double GetValueDouble(const string& node_name) {
			double value;

			if (NGetAsFloat(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::GetValueDouble, NGetAsFloat failed.");

			return value;
		}

		bool GetValueBoolean(const string& node_name) {
			cvbbool_t value;

			if (NGetAsBoolean(GetNode(node_name), value) < 0)
				throw std::runtime_error("Node::GetValueBoolean, NGetAsBoolean failed.");

			return value ? true : false;
		}

		string GetValueString(const string& node_name) {
			char value[256] = { 0 };
			size_t value_size = sizeof(value);

			if (NGetAsString(GetNode(node_name), value, value_size) < 0)
				throw std::runtime_error("Node::GetValueString, NGetAsString failed.");

			return value;
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

		void ListAllParameters(IMG cvb_image) {
			NODEMAP hNodeMap = NULL;
			cvbres_t result = NMHGetNodeMap(cvb_image, hNodeMap);
			cvbval_t value;
			NMNodeCount(hNodeMap, value);
			cerr << "Total parameters (NMNodeCount): " << value << endl;

			char name[256] = { 0 };
			size_t name_length = sizeof(name);

			for (int i = 0; i < value; i++) {
				NMListNode(hNodeMap, i, name, name_length);
				NODE node;
				NMGetNode(hNodeMap, name, node);
				NodeInfo(node);
			}
		}
	};

	class Parameter : public ParameterInterface {
	public:
		void Gain(double value) {
			SetValue("Gain", value);
		}

		double Gain() {
			return GetValueDouble("Gain");
		}

		void ExposureTime(double value) {
			SetValue("ExposureTime", value);
		}

		double ExposureTime() {
			return GetValueDouble("ExposureTime");
		}

		void AutoBrightness(bool value) {
			SetValue("autoBrightnessMode", (int)(value ? 1 : 0));
		}

		bool AutoBrightness() {
			return (GetValueString("autoBrightnessMode") == "Active") ? true : false;
		}

		void BalanceRatioRed(double value) {
			SetValue("BalanceRatioSelector", "Red");
			SetValue("BalanceRatio", value);
		}

		double BalanceRatioRed() {
			SetValue("BalanceRatioSelector", "Red");
			return GetValueDouble("BalanceRatio");
		}

		void BalanceRatioGreen(double value) {
			SetValue("BalanceRatioSelector", "Green");
			SetValue("BalanceRatio", value);
		}

		double BalanceRatioGreen() {
			SetValue("BalanceRatioSelector", "Green");
			return GetValueDouble("BalanceRatio");
		}

		void BalanceRatioBlue(double value) {
			SetValue("BalanceRatioSelector", "Blue");
			SetValue("BalanceRatio", value);
		}

		double BalanceRatioBlue() {
			SetValue("BalanceRatioSelector", "Blue");
			return GetValueDouble("BalanceRatio");
		}

		void TriggerMode(bool value) {
			SetValue("TriggerMode", (int)(value ? 1 : 0));
		}

		bool TriggerMode() {
			return (GetValueInteger("TriggerMode") == 1) ? true : false;
		}

		void TriggerSoftware() {
			SetValue("TriggerSoftware", true);
		}
	};
}
