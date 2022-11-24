#pragma once

#include "ThreadSafeFaceImageEvaluator.h"
#include "Data.h"
#include <curl/curl.h>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include <vector>
#include <crow.h>
#include <crow/utility.h>

class ServerBackend
{
public:
	ServerBackend(const int refreshTime);
	~ServerBackend();
	void init();
	void reg(const crow::request& req, crow::response& res);
	void enter(const crow::request& req, crow::response& res);
private:
	void updateLocalDatabase(const crow::json::rvalue& response);
	std::pair<size_t, double> compareImgWithDatabase(const cv::Mat& image);
	bool getDatabaseData(crow::json::rvalue& response);
	CURL* curl;
	ThreadSafeFaceImageEvaluator evaluator;
	std::vector<Data> local_db;
	std::mutex curl_mutex;
	boost::shared_mutex db_mutex;
};