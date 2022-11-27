#include "ServerBackend.h"
#include "utilities.h"
#include <crow.h>
#include <chrono>
#include <regex>
#include <thread>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

ServerBackend::ServerBackend(): evaluator("face_detection_yunet_2022mar.onnx", "face_recognition_sface_2021dec.onnx"), curl(nullptr)
{

}
ServerBackend::~ServerBackend()
{
	curl_easy_cleanup(curl);
}

void ServerBackend::init()
{
	curl = curl_easy_init();
	crow::json::rvalue database;
	this->getDatabaseData(database);
	this->updateLocalDatabase(database);
}

void ServerBackend::reg(const crow::request& req, crow::response& res)
{
	const std::string name = req.url_params.get("name");
	const std::string email = req.url_params.get("email");
	std::string base64 = req.url_params.get("faceimg");


	if (!ServerBackend::emailCheck(email))
	{
		res.write("Invalid e-mail format!");
		res.end();
		return;
	}

	if (!this->checkEmailIsUnique(email))
	{
		res.write("E-mail already in use!");
		res.end();
		return;
	}
	// Take out the unecessary part
	base64 = base64.substr(base64.find_first_of(",") + 1);
		
	// Encode from base64 to mat
	cv::Mat image = base64_utilities::str2mat(base64);
	cv::Mat faces;

	evaluator.findFaces(image, faces);
	if(faces.rows == 0)
	{
		res.write("There are no faces on the picture");
	}
	else if(faces.rows > 1)
	{
		res.write("There are too many faces on the picture!");
	}
	else
	{
		cv::Mat cropped_img;
		evaluator.alignCrop(image, faces.row(0), cropped_img);
		cv::Mat feature;
		boost::upgrade_lock<boost::shared_mutex> db_lock(db_mutex);
		evaluator.evaluateFeature(cropped_img, feature);

		const auto best_person = this->compareImgWithDatabase(feature);
		if(best_person.second > 0.363)
		{
			res.write("You are " + local_db[best_person.first].name + "! You have already registered! Confidence: " + std::to_string(best_person.second));
		}
		else
		{
			boost::upgrade_to_unique_lock unique_db_lock(db_lock);

			
			auto name_tmp = name;
			name_tmp.erase(std::remove(name_tmp.begin(), name_tmp.end(), ' '), name_tmp.end());
			const auto path = name_tmp + std::to_string(static_cast<int>(best_person.second * 100.0)) + ".jpg";
		
		   // Save the mat to the given folder
			base64_utilities::createFolderandMatFile(cropped_img, path);
	
			

			Data newData(local_db.size() + 1, name, email, path, false, feature);
			// save to local_db

			// Insert to the database
			const auto successful = insertDatabaseData(newData);
			if (successful)
			{
				local_db.push_back(newData);
				res.write("You have registered successfully!");
			}
			else
			{
				res.write("Unsuccessfull registration!");
			}
		}
	}
	res.end();
}

void ServerBackend::send(const crow::request& req, crow::response& res)
{
	crow::json::wvalue response;
	std::string base64 = req.url_params.get("faceimg");
	std::replace(base64.begin(), base64.end(), ' ', '+');

	auto imgMat = base64_utilities::str2mat(base64);
	cv::cvtColor(imgMat, imgMat, cv::COLOR_BGR2RGB);
	cv::Mat feature;
	evaluator.evaluateFeature(imgMat, feature);

	boost::shared_lock<boost::shared_mutex> db_lock(db_mutex);
	const auto best_person = this->compareImgWithDatabase(feature);
	if (best_person.second > 0.363)
	{
		if (local_db[best_person.first].registered)
		{
			response["status"] = 0;
			response["name"] = local_db[best_person.first].name;
			response["email"] = local_db[best_person.first].email;
		}
		else
		{
			response["status"] = 1;
			response["id"] = local_db[best_person.first].id;
			response["name"] = local_db[best_person.first].name;
			response["email"] = local_db[best_person.first].email;
		}
	}
	else
	{
		response["status"] = 2;
	}
	res.write(response.dump());
	res.end();

}

void ServerBackend::enter(const crow::request& req, crow::response& res)
{
	const auto id = std::stoi(req.url_params.get("id"));
	std::lock_guard<boost::shared_mutex> lock(db_mutex);
	const auto succesfull = updateDatabaseData(id);
	if (succesfull)
	{
		if (!local_db[id - 1].registered)
		{
			res.write("You can enter now!");
			local_db[id - 1].registered = true;
		}
		else
		{
			res.write("You have already entered this event!");
		}
	}
	else
	{
		res.write("There was a problem! Please contact our staff!");
	}
	res.end();
}

bool ServerBackend::getDatabaseData(crow::json::rvalue& response)
{
	if (curl)
		{
		curl_easy_reset(curl);
			CURLcode res;
			std::string readBuffer;
			curl_easy_setopt(curl, CURLOPT_URL, "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=get&select=id,name,email,pic,is_registered");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			if ( res != CURLE_OK)
			{
				std::cerr << "Could not reach database!" << std::endl;
				return false;
			}
			else
			{
				// need to substring to eliminate unecessary parts
				response = crow::json::load(readBuffer.substr(3, readBuffer.size()));
			}
		}
		else
		{

			std::cerr << "Curl does not exist!" << std::endl;
			return false;
		}
	return true;
}

void ServerBackend::updateLocalDatabase(const crow::json::rvalue& response)
{
	const auto items = response["data"];
	local_db.resize(items.size());
	for (size_t i = 0; i < local_db.size(); i++)
	{
		const auto data = items[i];
		
		cv::Mat image = cv::imread("FaceRecMatResults/" + std::string(data["pic"].s()));
		cv::Mat feature;
		evaluator.evaluateFeature(image, feature);
		local_db[i] = Data(data["id"].u(), std::string(data["name"].s()), std::string(data["email"].s()), std::string(data["pic"].s()), data["is_registered"].u() == 1, feature);
	}
}

bool ServerBackend::updateDatabaseData(const size_t id)
{
	std::unique_lock<std::mutex> lock(curl_mutex);
	std::string readBuffer;
	CURLcode result;
	std::string url = "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=put&select=is_registered=1&where=id=" + std::to_string(id);
	if (curl) {
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		const auto result = curl_easy_perform(curl);
		lock.unlock();
		if (result != CURLE_OK)
		{
			std::cerr << "Could not update database!" << std::endl;
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		std::cerr << "CURL does not exist!" << std::endl;
		return false;
	}
	
}

bool ServerBackend::insertDatabaseData(const Data& data)
{
	std::unique_lock<std::mutex> lock(curl_mutex);
	std::string readBuffer;
	CURLcode result;
	std::string url = "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=post&select=id,name,email,pic&values=" + ServerBackend::DataToString(data);
	url.erase(std::remove(url.begin(), url.end(),' '), url.end());
	curl = curl_easy_init();
	if (curl) {
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		result = curl_easy_perform(curl);
		lock.unlock();
		if (result == CURLE_OK)
		{
			return true;
		}
		else
		{
			std::cout << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(result);
			return false;
		}
	}
	return false;
}
std::pair<size_t, double> ServerBackend::compareImgWithDatabase(const cv::Mat& feature)
{
	size_t index = 0;
	double confidence = local_db.size() > 0 ? evaluator.match(feature, local_db.front().feature, cv::FaceRecognizerSF::DisType::FR_COSINE) : 0.0;
	for (size_t i = 1; i < local_db.size(); i++)
	{
		const auto current_confidence = evaluator.match(feature, local_db[i].feature, cv::FaceRecognizerSF::DisType::FR_COSINE);
		if (confidence < current_confidence)
		{
			confidence = current_confidence;
			index = i;
		}

	}
	return std::make_pair(index, confidence);
}

std::string ServerBackend::DataToString(const Data& data)
{
	return std::to_string(data.id) + ", \'" + data.name + "\', \'" + data.email + "\', \'" + data.file_path + "\'";
}
bool ServerBackend::emailCheck(const std::string& email)
{
	const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	return std::regex_match(email, pattern);
}

bool  ServerBackend::checkEmailIsUnique(const std::string& email)
{
	boost::shared_lock<boost::shared_mutex> lock(db_mutex);
	for (auto& data : local_db)
	{
		if (data.email == email) return false;
	}
	return true;

}

