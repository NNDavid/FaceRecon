#include "MainWindow.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), imageLabel(new QLabel(this)), dnn("opencv_face_detector_uint8.pb", "opencv_face_detector.pbtxt")
{
	this->setCentralWidget(imageLabel);
	try
	{
		videoCapture.open(0);
	}
	catch (...)
	{
		throw std::runtime_error("Camera could not be accessed by software!");
	}
	auto* refreshTimer = new QTimer(this);
	connect(refreshTimer, SIGNAL(timeout()), this, SLOT(showCamera()));
	refreshTimer->start(20);
}

void MainWindow::showCamera()
{
	if (videoCapture.isOpened())
	{
		cv::Mat image;
		videoCapture.read(image);
		std::vector<NeuralNetwork::Output> faces;
		dnn.evaluate(image, faces);
		for(auto& face : faces)
		{
			cv::rectangle(image, face.p1, face.p2, cv::Scalar(0, 255, 0),2, 4);
		}
		//std::cout << image << std::endl;
		cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
		imageLabel->setPixmap(QPixmap::fromImage(QImage(reinterpret_cast<uchar*>(image.data), image.cols, image.rows, image.step, QImage::Format_RGB888)));
	}
	else
	{
		throw std::runtime_error("Could not display video from camera!");
	}
}