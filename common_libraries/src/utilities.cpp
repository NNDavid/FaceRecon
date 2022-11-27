#include "utilities.h"
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <filesystem>
#include <iostream>


 bool base64_utilities::is_base64(unsigned char c)
 {
 	return (std::isalnum(c) || (c == '+') || (c == '/'));
 }

 void base64_utilities::createFolderandMatFile(const cv::Mat& imgMat, const std::string& path)
 {
	 // Testing the folder and file creation
	 auto ret2 = std::filesystem::create_directories("FaceRecMatResults");
	 if (ret2) {
		 std::cout << "created directory tree as follows: " << std::endl;
	 }
	 else {
		 std::cout << "create_directories() failed" << std::endl;
	 }

	 // Creating File
	 cv::imwrite("FaceRecMatResults/" + path, imgMat);
 }
 
 std::string base64_utilities::base64_decode(const std::string& encoded_string)
 {
 	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;

		if (i == 4)
		{
			for (i = 0; i < 4; i++)
			{
				char_array_4[i] = base64_chars.find(char_array_4[i]);
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
			{
				ret += char_array_3[i];
			}

			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
		{
			char_array_4[j] = 0;
		}

		for (j = 0; j < 4; j++)
		{
			char_array_4[j] = base64_chars.find(char_array_4[j]);
		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
		{
			ret += char_array_3[j];
		}
	}

	return ret;
 }


 cv::Mat base64_utilities::str2mat(const std::string& s)
 {
 	// Decode data
	std::string decoded_string = base64_utilities::base64_decode(s);
	std::vector<uchar> data(decoded_string.begin(), decoded_string.end());

	cv::Mat img = cv::imdecode(data, cv::IMREAD_UNCHANGED);
	return img;
 }

 std::string base64_utilities::base64_encode(uchar const* bytes_to_encode, unsigned int in_len) 
{
	std::string ret;

	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) 
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++) 
			{
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i) 
	{
		for (j = i; j < 3; j++) 
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++) 
		{
			ret += base64_chars[char_array_4[j]];
		}
		
		while ((i++ < 3)) 
		{
			ret += '=';
		}
	}

	return ret;
}

std::string base64_utilities::mat2str(const cv::Mat& m)
{
	int params[3] = {0};
	params[0] = cv::IMWRITE_JPEG_QUALITY;
	params[1] = 100;

	std::vector<uchar> buf;
	bool code = cv::imencode(".jpg", m, buf, std::vector<int>(params, params+2));
	uchar* result = reinterpret_cast<uchar*> (&buf[0]);

	return base64_encode(result, buf.size());
}
