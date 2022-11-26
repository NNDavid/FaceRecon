#pragma once
#include "FaceImageEvaluator.h"
#include <curl/curl.h>
#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <opencv2/videoio.hpp>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
private slots:
	void showCamera();
private:
	QLabel* imageLabel;
	QLabel* statusLabel;
	QLabel* faceLabel;
	cv::VideoCapture videoCapture;
	FaceImageEvaluator evaluator;
	size_t counter;
	CURL* curl;
};
