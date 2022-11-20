#include <crow.h>
#include <crow/utility.h>
#include <filesystem>
#include <curl/curl.h>
#include <cpprest/json.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include<direct.h>
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>
#include <string>

using namespace std;
using namespace cv;
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
//#include <../../../FaceReconDesktop/main.cpp>

static string readBuffer;
static 	bool img = false;
//https://stackoverflow.com/questions/39800992/cannot-listen-on-specific-port-num-with-localhost-address-with-http-listenerc

static const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	//cout<< " " << size << " " << nmemb  << endl;
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

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
	std::string ret;

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


cv::Mat str2mat(const string& s)
{
	// Decode data
	string decoded_string = base64_decode(s);
	vector<uchar> data(decoded_string.begin(), decoded_string.end());

	cv::Mat img = imdecode(data, IMREAD_UNCHANGED);
	return cv::Mat();
}

int main()
{
	crow::SimpleApp app;
	crow::mustache::set_base("/");
	CURL* curl;
	CURLcode res;
	string readBuffer;
	http_client client(U("http://localhost:18080/"));
	http_request req(methods::GET);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.eventshare.hu/v0.2/src/api/apiController.php?query_table=face_rec_event&query_type=get&select=pic");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		auto test = crow::json::load(readBuffer.substr(3, readBuffer.size()));
	  //  cout <<test << endl;
	}

    CROW_ROUTE(app, "/s")
		.methods("GET"_method, "POST"_method)
        ([](const crow::request& req, crow::response& res) {
		//CROW_LOG_INFO << "Params: " << req.url_params << "\n\n";
		img = true;

		cv::Mat testMat = str2mat(req.url_params.get("img"));

	 	//CROW_LOG_INFO << "Params: " << req.url_params.get("img") << "\n\n";
		//main(NULL,NULL);
	    //Creating File
	/*	if (mkdir("E:/MyFolder") == -1)
			cerr << " Error : " << strerror(errno) << endl;

		else
			cout << "File Created";

		ofstream MyFile("filename.txt");

		// Write to the file
		MyFile << "Files can be tricky, but it is fun enough!";

		// Close the file
		MyFile.close();*/
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

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be a https:// URL if that is what should receive the
		   data. */
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/");
		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();


	// sync request
	auto resp = client.request(req).get();
	cout << resp.status_code() << " : sync request" << endl;
}