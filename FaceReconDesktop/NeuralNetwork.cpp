#include "NeuralNetwork.h"
#include <opencv2/core.hpp>

NeuralNetwork::NeuralNetwork(const std::string& model_name, const std::string& config_name)
{
	network = cv::dnn::readNetFromTensorflow(model_name, config_name);
	network.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	network.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}

void NeuralNetwork::evaluate(const cv::Mat& input, /*output*/ std::vector<Output>& faceResult)
{
	faceResult.clear();
	const auto meanVal = cv::Scalar(104, 117, 123);
	const auto width = input.cols;
	const auto height = input.rows;
	const auto size = cv::Size(300, 300);
	const auto inputBlob = cv::dnn::blobFromImage(input, 1.0, size, meanVal, true, false);
	network.setInput(inputBlob, "data");
	auto result = network.forward("detection_out");
	cv::Mat faces(result.size[2], result.size[3], CV_32F, result.ptr<float>());

	for(uint32_t i = 0; i < faces.rows; ++i)
	{
		if(faces.at<float>(i, 2) < 0.8f) continue;
		int x1 = static_cast<int>(faces.at<float>(i, 3) * width);
        int y1 = static_cast<int>(faces.at<float>(i, 4) * height);
        int x2 = static_cast<int>(faces.at<float>(i, 5) * width);
        int y2 = static_cast<int>(faces.at<float>(i, 6) * height);
        faceResult.push_back({{x1, y1}, {x2, y2}});
	}
}