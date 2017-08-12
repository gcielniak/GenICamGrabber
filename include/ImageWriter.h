#pragma once

#include <locale>
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "SignalConnector.h"
#ifdef HAVE_PCL
#include <pcl/io/lzf_image_io.h>
#endif

// Get current date/time, format is YYYYMMDDTHHmmss
const std::string currentDateTime() {
	std::ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), new boost::posix_time::time_facet("%Y%m%dT%H%M%S")));
	ss << boost::posix_time::second_clock::local_time();
	return ss.str();
}

boost::posix_time::ptime from_us(long long us) {
	return boost::posix_time::from_time_t(us / 1000000) + boost::posix_time::microseconds(us % 1000000);
}

enum WriteFormat {
	PNG_FORMAT,
	TIFF_FORMAT,
	PCLZF_FORMAT
};

class ImageWriter {
	int counter = 0;
	WriteFormat format;
	std::string output_data_path;
	bool simulated_time;
	boost::posix_time::ptime sensor_tstart, sensor_timestamp_first;

public:
	ImageWriter() :
		output_data_path(".\\data\\" + currentDateTime() + "\\"), format(PNG_FORMAT), simulated_time(false) {
	}

	WriteFormat Format() { return format; }
	void Format(WriteFormat value) { format = value; }

	void AvailableFormats(std::vector<WriteFormat>& format_list) {
		format_list.push_back(PNG_FORMAT);
		format_list.push_back(TIFF_FORMAT);
#ifdef HAVE_PCL
		format_list.push_back(PCLZF_FORMAT);
#endif
	}

	std::string OutputDir() { return output_data_path; }
	void OutputDir(const std::string& new_path) { output_data_path = new_path; }

	void SimulatedTime(bool value) { simulated_time = value; }
	bool SimulatedTime() { return simulated_time; }

	std::string TimeString(long long timestamp) {
		//image timestamp: can be a timestamp from a sensor (in relative units) or simulated from a filename
		boost::posix_time::ptime image_timestamp = from_us(timestamp);

		//if timestamp from sensor then calculate offset from the current time
		if (!simulated_time) {
			if (sensor_timestamp_first.is_not_a_date_time()) {//first frame
				sensor_tstart = boost::posix_time::microsec_clock::local_time();
				sensor_timestamp_first = image_timestamp;
			}
			//correct
			image_timestamp += sensor_tstart - sensor_timestamp_first;
		}

		return boost::posix_time::to_iso_string(image_timestamp);
	}

	virtual void operator()(const Image& image) {

		std::string filename = "frame_" + TimeString(image.timestamp);

		if (!boost::filesystem::exists(boost::filesystem::path(output_data_path)))
			boost::filesystem::create_directories(boost::filesystem::path(output_data_path));

		cv::Mat image_rgb;
		std::vector<cv::Mat> channels;

		switch (image.type) {
		case RGBN_IMAGE:
			filename += "_rgbn";
			break;
		default:
			break;
		}

		switch (format) {
		case PNG_FORMAT:
			filename += ".png";
			cv::imwrite(output_data_path + filename, image.image, { CV_IMWRITE_PNG_COMPRESSION, 0});
			break;
		case TIFF_FORMAT:
			filename += ".tif";
			cv::imwrite(output_data_path + filename, image.image);
			break;
#ifdef HAVE_PCL
		case PCLZF_FORMAT: {
			pcl::io::LZFRGB24ImageWriter rgb_writer;
			pcl::io::LZFBayer8ImageWriter nir_writer;
			std::vector<cv::Mat> channels;
			cv::split(image.image, channels); // break image into channels
			cv::Mat image_rgb;
			cv::merge({ { channels[0], channels[1], channels[2] } }, image_rgb); // combine rgb channels only
			rgb_writer.write((char*)(image_rgb.data), image_rgb.cols, image_rgb.rows, output_data_path + "frame_" + TimeString(image.timestamp) + "_rgb.pclzf");
			nir_writer.write((char*)channels[3].data, channels[3].cols, channels[3].rows, output_data_path + "frame_" + TimeString(image.timestamp) + "_nir.pclzf");
		}
			break;
#endif
		default:
			break;
		}
	}
};


