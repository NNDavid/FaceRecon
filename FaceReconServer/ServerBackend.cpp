#include "ServerBackend.h"
#include "utilities.h"
#include <chrono>
#include <thread>

ServerBackend::ServerBackend(const int refreshTime): evaluator("face_detection_yunet_2022mar.onnx", "face_recognition_sface_2021dec.onnx")
{
	std::thread refresh_thread(&ServerBackend::refreshData, this, refreshTime);
	refresh_thread.detach();
}

void ServerBackend::refreshData(const int refreshTime)
{
	while(true)
	{
		std::unique_lock<std::mutex> curl_lock(curl_mutex);
		// TODO: perform db request
		curl_lock.unlock();
		std::unique_lock<boost::shared_mutex> db_lock(db_mutex);
		// TODO: perform local update
		db_lock.unlock();
		std::this_thread::sleep_for(std::chrono::minutes(refreshTime));
	}
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
		boost::shared_lock<boost::shared_mutex> db_lock(db_mutex);
		// TODO: compare with database
		db_lock.unlock();
	}
}

void ServerBackend::enter(const crow::request& req, crow::response& res)
{
	std::string base64 = req.url_params.get("faceimg");

	// Take out the unecessary part
	base64 = base64.substr(base64.find_first_of(",") + 1);
		
	// Encode from base64 to mat
	cv::Mat imgMat = base64_utilities::str2mat(base64);
	boost::shared_lock<boost::shared_mutex> db_lock(db_mutex);
	// TODO: Find faces that match in local db
	db_lock.unlock();
}

