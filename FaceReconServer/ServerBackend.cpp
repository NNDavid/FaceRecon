#include "ServerBackend.h"
#include "utilities.h"
#include <crow.h>
#include <chrono>
#include <thread>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

ServerBackend::ServerBackend(): evaluator("face_detection_yunet_2022mar.onnx", "face_recognition_sface_2021dec.onnx")
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
		cv::Mat feature;
		evaluator.evaluateFeature(cropped_img, feature);
		const auto best_person = this->compareImgWithDatabase(feature);
		if(best_person.second > 0.363)
		{
			res.write("You have already registered!");
		}
		else
		{
			boost::upgrade_to_unique_lock unique_db_lock(db_lock);

			std::string name = req.url_params.get("name");
			std::string email = req.url_params.get("email");
			auto name_tmp = name;
			name_tmp.erase(std::remove(name_tmp.begin(), name_tmp.end(), ' '), name_tmp.end());
			const auto path = name_tmp + std::to_string(static_cast<int>(best_person.second * 100.0)) + ".jpg";
		
		   // Save the mat to the given folder
			base64_utilities::createFolderandMatFileconst(cropped_img, path);
	
			

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

void ServerBackend::enter(const crow::request& req, crow::response& res)
{
	std::string base64 = req.url_params.get("faceimg");

	// Take out the unecessary part
	//base64 = base64.substr(base64.find_first_of(",") + 1);
		
	// Encode from base64 to mat
	cv::Mat imgMat = base64_utilities::str2mat(base64);
	cv::Mat feature;
	evaluator.evaluateFeature(imgMat, feature);

	boost::upgrade_lock<boost::shared_mutex> db_lock(db_mutex);
	const auto best_person = this->compareImgWithDatabase(imgMat);
	if (best_person.second > 0.363)
	{
		if (local_db[best_person.second].registered)
		{
			res.write(" You are " + local_db[best_person.second].name + "! You have already entered this event!");
		}
		else
		{
			boost::upgrade_to_unique_lock unique_db_lock(db_lock);
			local_db[best_person.second].registered = true;
			res.write(" You are " + local_db[best_person.second].name + "! You may enter!");
			
			updateDatabaseData(local_db[best_person.second].id);
		}
	}
	else
	{
		res.write("You are not registered!");
	}

}

bool ServerBackend::getDatabaseData(crow::json::rvalue& response)
{
	curl = curl_easy_init();
	if (curl)
		{
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
		std::cout << "new_item  = " << data << std::endl;
		local_db[i] = Data(data["id"].u(), std::string(data["name"].s()), std::string(data["email"].s()), std::string(data["pic"].s()), data["is_registered"].u() == 1);
		cv::Mat image = cv::imread("FaceRecMatResults/" +  local_db[i].file_path);
		evaluator.evaluateFeature(image, local_db[i].feature);
	}
}

void ServerBackend::updateDatabaseData(const size_t id)
{
	std::unique_lock<std::mutex> lock(curl_mutex);
	std::string readBuffer;
	CURLcode result;
	std::string url = "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=put&select=is_registered=0&where=id=" + std::to_string(id);
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		const auto result = curl_easy_perform(curl);
		lock.unlock();
		if (result != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result));
		curl_easy_cleanup(curl);
	}
	else {
		lock.unlock();
	}
	
}

bool ServerBackend::insertDatabaseData(const Data& data)
{
	std::unique_lock<std::mutex> lock(curl_mutex);
	std::string readBuffer;
	CURLcode result;
	std::string url = "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=post&select=name,email,pic&values=" + ServerBackend::DataToString(data);
	url.erase(std::remove(url.begin(), url.end(),' '), url.end());
	curl = curl_easy_init();
	if (curl) {
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
	for(size_t i = 1; i < local_db.size(); i++)
	{
		if(!local_db[i].registered)
		{
			const auto current_confidence = evaluator.match(feature, local_db[i].feature, cv::FaceRecognizerSF::DisType::FR_COSINE);
			if(confidence < current_confidence)
			{
				confidence = current_confidence;
				index  = i;
			}
		}

	}
	return std::make_pair(index, confidence);
}

std::string ServerBackend::DataToString(const Data& data)
{
	return "\'" + data.name + "\', \'" + data.email + "\', \'" + data.file_path + "\'";
}

