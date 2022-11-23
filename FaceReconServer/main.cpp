#include "utilities.h"
#include <crow.h>
#include <crow/utility.h>
#include <filesystem>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <direct.h>
#include <string>

// Callback function for the curl result
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void getData() 
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=get&select=pic");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		auto test = crow::json::load(readBuffer.substr(3, readBuffer.size()));
	}
}

void insertData()
{
	CURL* curl;
	static std::string readBuffer;
	static CURLcode result;

	curl = curl_easy_init();
	std::string test = "'proba'";
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=post&select=name,email,pic&values='gre','dsa.com'," + test);
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}

void createFolderandMatFile(cv::Mat& imgMat)
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
	cv::imwrite("FaceRecMatResults/test.bmp", imgMat);
}

int main()
{
	crow::SimpleApp app;


	CROW_ROUTE(app, "/submit")
		.methods("GET"_method)
		([](const crow::request& req, crow::response& res) {

		// Get the image from the request params
		std::string base64 = req.url_params.get("faceimg");

		// Take out the unecessary part
		base64 = base64.substr(base64.find_first_of(",") + 1);
		
		// Encode from base64 to mat
		cv::Mat imgMat = base64_utilities::str2mat(base64);

		// Save the mat to the given folder
		createFolderandMatFile(imgMat);

		// Response to the website
		res.write("Sent image!");
		res.end();
		});

	CROW_ROUTE(app, "/")([] {
		CROW_LOG_INFO << std::filesystem::current_path() << "\n\n";
		return crow::mustache::load("index.php").render();
		});

	app.port(18080)
		.multithreaded()
		.run();
}
