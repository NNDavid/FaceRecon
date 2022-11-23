#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

// Source: https://github.com/RonnyldoSilva/OpenCV_Mat_to_Base64
class base64_utilities
{
public:
	static cv::Mat str2mat(const std::string& s);
	static std::string mat2str(const cv::Mat& img);

private:
	static bool is_base64(unsigned char c);
	static std::string base64_decode(const std::string& encoded_string);
	static std::string base64_encode(uchar const* bytesToEncode, unsigned int inLen);
	static inline const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
};