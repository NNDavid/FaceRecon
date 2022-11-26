#pragma once
#include "FaceImageEvaluator.h"
#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <curl/curl.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();
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
