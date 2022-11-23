#include "MainWindow.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>
#include <opencv2/imgproc.hpp>
#include <stdexcept>
#include <algorithm>

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
	statusLabel = new QLabel("All OK", this);
	faceLabel = new QLabel(this);
	statusBar()->addWidget(statusLabel);
	statusBar()->addWidget(faceLabel);
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
		cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
		std::vector<NeuralNetwork::Output> faces;
		dnn.evaluate(image, faces);
		if (faces.size() == 0)
		{
			statusLabel->setText("There are no faces in the picture!");
		}
		else if (faces.size() == 1)
		{
			const auto face = faces.front();

			const auto x_min = std::max(std::min(face.p1.x, face.p2.x), 0);
			const auto x_length = std::min(std::abs(face.p2.x - face.p1.x), image.cols - x_min);

			const auto y_min = std::max(std::min(face.p1.y, face.p2.y), 0);
			const auto y_length = std::min(std::abs(face.p2.y - face.p1.y), image.rows - y_min);

			auto cropped_img = image(cv::Rect(x_min, y_min, x_length, y_length));
			faceLabel->setPixmap(QPixmap::fromImage(QImage(reinterpret_cast<uchar*>(cropped_img.data), cropped_img.cols, cropped_img.rows, cropped_img.step, QImage::Format_RGB888)));
			statusLabel->setText("Face found!");
		}
		else
		{
			statusLabel->setText("There are too many faces in the picture!");
		}
		for (auto& face : faces)
		{
			cv::rectangle(image, face.p1, face.p2, cv::Scalar(0, 255, 0), 2, 4);
		}
		imageLabel->setPixmap(QPixmap::fromImage(QImage(reinterpret_cast<uchar*>(image.data), image.cols, image.rows, image.step, QImage::Format_RGB888)));
	}
	else
	{
		throw std::runtime_error("Could not display video from camera!");
	}
}