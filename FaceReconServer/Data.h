#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

struct Data
{
	Data(const size_t id, const std::string& name, const std::string& email, const std::string& file_path, const bool registered = false, const cv::Mat& feature = cv::Mat()):
		id(id),
		name(name),
		email(email),
		file_path(file_path),
		registered(registered),
	    feature(feature.clone()) {}
	Data(const Data& other):id(other.id), name(other.name), email(other.email), file_path(other.file_path), registered(other.registered), feature(other.feature.clone()) {}
	Data() = default;
	Data& operator=(const Data& other)
	{
		this->id = other.id;
		this->name = other.name;
		this->email = other.email;
		this->file_path = other.file_path;
		this->registered = other.registered;
		this->feature = other.feature.clone();
		return *this;
	}
	size_t id;
	std::string name;
	std::string email;
	std::string file_path;
	bool registered;
	cv::Mat feature;
};