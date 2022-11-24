#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

struct Data
{
	Data(const size_t id, const std::string& name, const std::string& email, const std::string& file_path, const bool registered = false) :
		id(id),
		name(name),
		email(email),
		file_path(file_path),
		registered(registered) {}
	Data() = default;
	size_t id;
	std::string name;
	std::string email;
	std::string file_path;
	bool registered;
	cv::Mat feature;
};