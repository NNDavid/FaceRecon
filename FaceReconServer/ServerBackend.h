#pragma once

#include "ThreadSafeNeuralNetwork.h"
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <mutex>
#include <crow.h>
#include <crow/utility.h>

class ServerBackend
{
public:
	ServerBackend(const int refreshTime);
	void reg(const crow::request& req, crow::response& res);
	void enter(const crow::request& req, crow::response& res);
private:
	void refreshData(const int refreshTime);
	ThreadSafeNeuralNetwork dnn;
	std::mutex curl_mutex;
	boost::shared_mutex db_mutex;
};