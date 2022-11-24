#include "utilities.h"
#include <crow.h>
#include <crow/utility.h>
#include <filesystem>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <direct.h>

// Callback function for the curl result
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/*
	UpdateData()
	Code format
	Theadsafe
	Data -> cv::mat feature needs to be added
	insertdata check if already inserted
	check file is already created
*/
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

void insertData(std::string& name, std::string& email, std::string& path)
{
	CURL* curl;
	std::string readBuffer;
	CURLcode result;
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);
	std::string url = "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=post&select=name,email,pic&values=" + name + ',' + email + ',' + path;
	url.erase(std::remove(url.begin(), url.end(), ' '), url.end());
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		result = curl_easy_perform(curl);
		if (result != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result));
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}


void createFolderandMatFile(cv::Mat& imgMat, std::string& path)
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
	cv::imwrite("FaceRecMatResults/" + path + ".bmp", imgMat);
}

int main()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/submit")
		.methods("GET"_method)
		([](const crow::request& req, crow::response& res) {

		// Get the image from the request params
		std::string base64 =  req.url_params.get("faceimg");
		std::string name = req.url_params.get("name");
		std::string email = req.url_params.get("email");
		std::string path = req.url_params.get("imgPath");

		// Take out the unecessary part
		base64 = base64.substr(base64.find_first_of(",") + 1);
		
		// Encode from base64 to mat
		cv::Mat imgMat = base64_utilities::str2mat(base64);

		// Save the mat to the given folder
		createFolderandMatFile(imgMat, path);

		// Insert to the database
		insertData("\'" + name + "\'","\'" + email + "\'","\'" + path + "\'");

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
