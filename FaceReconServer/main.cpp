#include <crow.h>
#include <crow/utility.h>
#include <filesystem>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <direct.h>
#include <string>

using namespace std;
using namespace cv;

// Callback function for the curl result
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// Check if it is a real base64 code
static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_decode(string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	string ret;
    const string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

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


cv::Mat str2mat(const std::string& s)
{
	// Decode data
	string decoded_string = base64_decode(s);
	vector<uchar> data(decoded_string.begin(), decoded_string.end());

	cv::Mat img = imdecode(data, IMREAD_UNCHANGED);
	// Check if the mat image is the same as base64
	imshow("Image", img);
	waitKey();
	return img;
}

void getData() 
{
	CURL* curl;
	CURLcode res;
	string readBuffer;

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
	static string readBuffer;
	static CURLcode result;

	curl = curl_easy_init();
	string test = "'proba'";
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
	auto ret2 = filesystem::create_directories("FaceRecMatResults");
	if (ret2) {
		cout << "created directory tree as follows: " << endl;
	}
	else {
		cout << "create_directories() failed" << endl;
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
		string base64 = req.url_params.get("faceimg");

		// Take out the unecessary part
		base64 = base64.substr(base64.find_first_of(",") + 1);
		
		// Encode from base64 to mat
		cv::Mat imgMat = str2mat(base64);

		// Save the mat to the given folder
		createFolderandMatFile(imgMat);

		// Response to the website
		res.write("Sent image!");
		res.end();
		});

	CROW_ROUTE(app, "/")([] {
		CROW_LOG_INFO << filesystem::current_path() << "\n\n";
		return crow::mustache::load("index.php").render();
		});

	app.port(18080)
		.multithreaded()
		.run();
}
