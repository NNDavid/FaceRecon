#include "ServerBackend.h"
#include "utilities.h"
#include <chrono>
#include <thread>

ServerBackend::ServerBackend(const int refreshTime): dnn("opencv_face_detector_uint8.pb", "opencv_face_detector.pbtxt")
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
	std::vector<ThreadSafeNeuralNetwork::Output> faces;
	dnn.evaluate(image, faces);
	if(faces.size() == 0)
	{
		res.write("There are no faces on the picture");
	}
	else if(faces.size() > 1)
	{
		res.write("There are too many faces on the picture!");
	}
	else
	{
		const auto face = faces.front();

		const auto x_min = std::max(std::min(face.p1.x, face.p2.x), 0);
		const auto x_length = std::min(std::abs(face.p2.x - face.p1.x), image.cols - x_min);

		const auto y_min = std::max(std::min(face.p1.y, face.p2.y), 0);
		const auto y_length = std::min(std::abs(face.p2.y - face.p1.y), image.rows - y_min);

		auto cropped_img = image(cv::Rect(x_min, y_min, x_length, y_length));
		cv::resize(cropped_img, cropped_img, cv::Size(256, 256), 0, 0);
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

