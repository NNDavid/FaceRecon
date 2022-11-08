#pragma once
#include "NeuralNetwork.h"
#include <QMainWindow>
#include <QLabel>
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
	cv::VideoCapture videoCapture;
	NeuralNetwork dnn;
};
