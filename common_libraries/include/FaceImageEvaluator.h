#pragma once
#include <opencv2/objdetect.hpp>

class FaceImageEvaluator
{
public:
	FaceImageEvaluator(const std::string& detector_path, const std::string& recognizer_path);
	virtual void findFaces(const cv::Mat& image, cv::Mat& faces);
	void alignCrop(const cv::Mat& image, const cv::Mat& face, cv::Mat& alignedFace) const;
	virtual void evaluateFeature(const cv::Mat& image, cv::Mat& feature);
	double match(const cv::Mat& feature1, const cv::Mat& feature2, int dis_type = cv::FaceRecognizerSF::FR_COSINE) const;

protected:
	cv::Ptr<cv::FaceDetectorYN> detector;
	cv::Ptr<cv::FaceRecognizerSF> recognizer;
};