#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <mutex>

class ThreadSafeNeuralNetwork
{
public:
	struct Output
	{
	   Output(const cv::Point& _p1, const cv::Point& _p2) : p1(_p1), p2(_p2) {}
	   cv::Point p1;
	   cv::Point p2;
   };
   ThreadSafeNeuralNetwork();
	ThreadSafeNeuralNetwork(const std::string& model_name, const std::string& config_name);
	void evaluate(const cv::Mat& input, /*output*/ std::vector<Output>& result);

private:
	cv::dnn::Net network;
	std::mutex eval_mutex;
};