#include "ServerBackend.h"
#include "utilities.h"
#include <crow.h>
#include <chrono>
#include <thread>
#include <opencv2/imgcodecs.hpp>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

ServerBackend::ServerBackend(const int refreshTime): evaluator("face_detection_yunet_2022mar.onnx", "face_recognition_sface_2021dec.onnx")
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
	std::string base64 = req.url_params.get("faceimg");

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
		boost::upgrade_lock<boost::shared_mutex> db_lock(db_mutex);
		const auto best_person = this->compareImgWithDatabase(cropped_img);
		if(best_person.second > 0.8)
		{
			res.write("You have already registered!");
		}
		else
		{
			boost::upgrade_to_unique_lock unique_db_lock(db_lock);
			//TODO insert to server (save picture!)
			//TODO push_back to local_db
			res.write("You have registered successfully!");
		}
	}
}

void ServerBackend::enter(const crow::request& req, crow::response& res)
{
	std::string base64 = req.url_params.get("faceimg");

	// Take out the unecessary part
	base64 = base64.substr(base64.find_first_of(",") + 1);
		
	// Encode from base64 to mat
	cv::Mat imgMat = base64_utilities::str2mat(base64);

	boost::upgrade_lock<boost::shared_mutex> db_lock(db_mutex);
	const auto best_person = this->compareImgWithDatabase(imgMat);
	if (best_person.second > 0.8)
	{
		boost::upgrade_to_unique_lock unique_db_lock(db_lock);
		if (local_db[best_person.second].registered)
		{
			res.write(" You are " + local_db[best_person.second].name + "! You have already entered this event!");
		}
		else
		{
			local_db[best_person.second].registered = true;
			res.write(" You are " + local_db[best_person.second].name + "! You may enter!");
			//TODO modify database in cloud
		}
	}
	else
	{
		res.write("You are not registered!");
	}

}

bool ServerBackend::getDatabaseData(crow::json::rvalue& response)
{
	std::unique_lock<std::mutex> lock(curl_mutex);
	if (curl)
		{
			CURLcode res;
			std::string readBuffer;
			curl_easy_setopt(curl, CURLOPT_URL, "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=get&select=pic");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			const auto result = curl_easy_perform(curl);
			lock.unlock();
			if ( res != CURLE_OK)
			{
				std::cerr << "Could not reach database!" << std::endl;
				return false;
			}
			else
			{
				response = crow::json::load(readBuffer.substr(3, readBuffer.size()));
			}
		}
		else
		{
			lock.unlock();
			std::cerr << "Curl does not exist!" << std::endl;
			return false;
		}
	return true;
}

void ServerBackend::updateLocalDatabase(const crow::json::rvalue& response)
{
	local_db.resize(response.size());
	for(size_t i = 0; i < response.size(); i++)
	{
		const auto data = response[i];
		//local_db[i] = Data(data["name"], data["email"], data["pic"], data["registered"]);
		cv::Mat image = cv::imread(local_db[i].file_path);
		evaluator.evaluateFeature(image ,local_db[i].feature);
	}
}

std::pair<size_t, double> ServerBackend::compareImgWithDatabase(const cv::Mat& feature)
{
	size_t index = 0;
	double confidence = local_db.size() > 0 ? evaluator.match(feature, local_db.front().feature) : 0.0;
	for(size_t i = 1; i < local_db.size(); i++)
	{
		if(!local_db[i].registered)
		{
			const auto current_confidence = evaluator.match(feature, local_db[i].feature); 
			if(confidence < current_confidence)
			{
				confidence = current_confidence;
				index  = i;
			}
		}

	}
	return std::make_pair(index, confidence);
}

