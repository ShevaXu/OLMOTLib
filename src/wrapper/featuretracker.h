#ifndef _OLMOT_FEATURE_TRACKER_H
#define _OLMOT_FEATURE_TRACKER_H

#include <opencv2//features2d/features2d.hpp>
//#include <opencv2/nonfree/features2d.hpp>

#include "../core/frametracker.h"

class FeatureTracker: public FrameTracker<cv::Mat, cv::Mat>
{
public:
	FeatureTracker(float windowFactor = 1.5f, bool verbose = true);

	virtual int init(const cv::Mat &frame, cv::Rect bb);	

	virtual int update(const cv::Mat &frame, cv::Rect bb);

	virtual cv::Rect track(const cv::Mat &frame, cv::Rect prevBB);	

	// return the transformation matrix
	cv::Mat getInfo() { return m_trans; }	

protected:
	std::vector<cv::DMatch> filterGoodMatches(const std::vector<cv::DMatch> &matches,
		int method = 0, double maxDist = 2.0, int nTimes = 3,
		double percentage = 0.4, int nDesire = 10);
	

protected:
	// data
	cv::Mat m_trans;
	std::vector<cv::KeyPoint> m_fromKPs, m_toKPs;
	cv::Mat m_fromDes, m_toDes;
	// tools
	cv::Ptr<cv::Feature2D> m_feat2D;
	cv::Ptr<cv::FeatureDetector> m_detector;
	cv::Ptr<cv::DescriptorExtractor> m_extractor;
	cv::Ptr<cv::DescriptorMatcher> m_matcher;
	
};

#endif