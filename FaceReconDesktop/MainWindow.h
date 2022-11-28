#pragma once
#include "FaceImageEvaluator.h"
#include <crow.h>
#include <curl/curl.h>
#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <QTimer>
#include <QPushButton>
#include <opencv2/videoio.hpp>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
private slots:
	void showCamera();
	void acceptEnter();
	void declineEnter();
private:
	void handleResponse(const crow::json::rvalue& response);
	QLabel* imageLabel;
	QLabel* statusLabel;
	QLabel* faceLabel;
	QLabel* responseLabel;
	QPushButton* acceptButton;
	QPushButton* declineButton;
	QTimer* refreshTimer;
	cv::VideoCapture videoCapture;
	FaceImageEvaluator evaluator;
	size_t counter;
	size_t ID;
	CURL* curl;
};
