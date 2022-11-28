#pragma once

#include "FaceImageEvaluator.h"
#include <opencv2/objdetect.hpp>
#include <mutex>

class ThreadSafeFaceImageEvaluator: public FaceImageEvaluator
{
public:
	ThreadSafeFaceImageEvaluator(const std::string& detector_path, const std::string& recognizer_path);
	void findFaces(const cv::Mat& image, cv::Mat& faces) final;
	void evaluateFeature(const cv::Mat& image, cv::Mat& feature) final;

private:
	cv::Ptr<cv::FaceRecognizerSF> evaluator;
	std::mutex detector_mutex;
	std::mutex evaluator_mutex;
};