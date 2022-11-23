#pragma once
#include "NeuralNetwork.h"
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
	NeuralNetwork dnn;
};
