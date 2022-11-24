#include "ThreadSafeFaceImageEvaluator.h"
#include <opencv2/core.hpp>

ThreadSafeFaceImageEvaluator::ThreadSafeFaceImageEvaluator(const std::string& detector_path, const std::string& recognizer_path):
	FaceImageEvaluator(detector_path, recognizer_path),
	evaluator(cv::FaceRecognizerSF::create(recognizer_path, "")) {}

void ThreadSafeFaceImageEvaluator::findFaces(const cv::Mat& image, cv::Mat& faces)
{
	std::lock_guard<std::mutex> lock(detector_mutex);
	FaceImageEvaluator::findFaces(image, faces);
}


void ThreadSafeFaceImageEvaluator::evaluateFeature(const cv::Mat& alignedFace, cv::Mat& feature)
{
	std::lock_guard<std::mutex> lock(evaluator_mutex);
	return evaluator->feature(alignedFace, feature);
}