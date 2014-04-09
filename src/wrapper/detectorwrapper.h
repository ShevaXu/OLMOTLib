#ifndef _OLMOT_DETECTOR_WRAPPER_H
#define _OLMOT_DETECTOR_WRAPPER_H

#include <opencv2/objdetect/objdetect.hpp>

#include "../core/detector.h"

class HOGPeopleDetector: public Detector
{
public:
	HOGPeopleDetector();

	// implemented
	MOTDetections getDetections(const cv::Mat &img, int fIdx);

private:
	cv::Ptr<cv::HOGDescriptor> m_hog;
};

class LatentSVMWrapper: public Detector
{
public:
	LatentSVMWrapper(const std::vector<std::string>& filenames, float confThresh = 0.5);	

	// implemented
	MOTDetections getDetections(const cv::Mat &img, int fIdx);

private:
	float m_confThresh;
	cv::Ptr<cv::LatentSvmDetector> m_detector;
};

class CascadeDetector: public Detector
{
public:
	CascadeDetector(const std::string& filename);

	// implemented
	MOTDetections getDetections(const cv::Mat &img, int fIdx);

private:
	cv::Ptr<cv::CascadeClassifier> m_cascade;
};


#endif	// detectorwrapper.h