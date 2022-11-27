#include "utilities.h"
#include <crow.h>
#include <crow/utility.h>
#include <filesystem>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <direct.h>
#include "ServerBackend.h"

ServerBackend serverbackend;

int main()
{
	crow::SimpleApp app;
	serverbackend.init();

	CROW_ROUTE(app, "/submit")
		.methods("GET"_method)
	([](const crow::request& req, crow::response& res) {
		serverbackend.reg(req, res);
	});

	CROW_ROUTE(app, "/send")
		.methods("GET"_method)
	([](const crow::request& req, crow::response& res) {
		// Forward to the server backend
		serverbackend.send(req, res);
	});

	CROW_ROUTE(app, "/enter")
		.methods("GET"_method)
		([](const crow::request& req, crow::response& res) {
		// Forward to the server backend
		serverbackend.enter(req, res);
			});

	CROW_ROUTE(app, "/")([] {
		CROW_LOG_INFO << std::filesystem::current_path() << "\n\n";
		return crow::mustache::load("index.php").render();
	});

	app.port(18080)
		.multithreaded()
		.run();
}
