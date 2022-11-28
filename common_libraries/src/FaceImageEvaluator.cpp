#include "FaceImageEvaluator.h"
#include <opencv2/core.hpp>

FaceImageEvaluator::FaceImageEvaluator(const std::string& detector_path, const std::string& recognizer_path):
	detector(cv::FaceDetectorYN::create(detector_path, "", cv::Size(320, 320), 0.9, 0.3, 5000)),
	recognizer(cv::FaceRecognizerSF::create(recognizer_path, "")) {}

void FaceImageEvaluator::findFaces(const cv::Mat& image, cv::Mat& faces)
{
	detector->setInputSize(image.size());
    detector->detect(image, faces);
}

void FaceImageEvaluator::alignCrop(const cv::Mat& image, const cv::Mat& face, cv::Mat& alignedFace) const
{
	recognizer->alignCrop(image, face, alignedFace);
}

void FaceImageEvaluator::evaluateFeature(const cv::Mat& alignedFace, cv::Mat& feature)
{
	recognizer->feature(alignedFace, feature);
}

double FaceImageEvaluator::match(const cv::Mat& feature1, const cv::Mat& feature2, int dis_type) const
{
	return recognizer->match(feature1, feature2, dis_type);
}