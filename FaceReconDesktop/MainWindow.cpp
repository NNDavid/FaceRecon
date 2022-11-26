#include "MainWindow.h"
#include "utilities.h"
#include <QVBoxLayout>
#include <QTimer>
#include <opencv2/imgproc.hpp>
#include <QImage>
#include <stdexcept>
#include <algorithm>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), imageLabel(new QLabel(this)), evaluator("face_detection_yunet_2022mar.onnx", "face_recognition_sface_2021dec.onnx"), counter(0)
{
	this->setCentralWidget(imageLabel);
	try
	{
		videoCapture.open(1);
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
	curl = curl_easy_init();
}

MainWindow::~MainWindow()
{
	curl_easy_cleanup(curl);
}

void MainWindow::showCamera()
{
	if (videoCapture.isOpened())
	{
		cv::Mat image;
		videoCapture.read(image);
		//cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
		cv::Mat faces;
		evaluator.findFaces(image, faces);
		if (faces.rows == 0)
		{
			statusLabel->setText("There are no faces in the picture!");
			counter = 0;
		}
		else if (faces.rows == 1)
		{
			++counter;
			cv::Mat cropped_img;
			evaluator.alignCrop(image, faces.row(0), cropped_img);
			faceLabel->setPixmap(QPixmap::fromImage(QImage(reinterpret_cast<uchar*>(cropped_img.data), cropped_img.cols, cropped_img.rows, cropped_img.step, QImage::Format_RGB888)));
			statusLabel->setText("Face found!");
			if (counter > 200)
			{
				std::string readBuffer;
				CURLcode result;
				// Encode from base64 to mat
				const auto base64Img = base64_utilities::mat2str(cropped_img);
				std::string url = "http://localhost:18080/send?faceimg=" + base64Img;
				if (curl) {
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
					auto result = curl_easy_perform(curl);
					if (result != CURLE_OK)
					{
						fprintf(stderr, "curl_easy_perform() failed: %s\n",
							curl_easy_strerror(result));
						statusLabel->setText("Face is NOT getting sent!!");
					}
					else statusLabel->setText("Face is getting sent!!");
				}
				else {
				}
				counter = 0;
			}
		}
		else
		{
			counter = 0;
			statusLabel->setText("There are too many faces in the picture!");
		}
		for (int i = 0; i < faces.rows; i++)
		{
			//cv::rectangle(image, cv::Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))), cv::Scalar(0, 255, 0), 2);
		}
		imageLabel->setPixmap(QPixmap::fromImage(QImage(reinterpret_cast<uchar*>(image.data), image.cols, image.rows, image.step, QImage::Format_RGB888)));
	}
	else
	{
		throw std::runtime_error("Could not display video from camera!");
	}
}