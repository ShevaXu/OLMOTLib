#ifndef _OLMOT_MEAN_SHIFT_TRACKER_H
#define _OLMOT_MEAN_SHIFT_TRACKER_H

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../core/frametracker.h"

class MeanShiftTracker: public FrameTracker<cv::Mat, int>
{
public:
	// both modes are not used now
	MeanShiftTracker(int colorMode, int weightingMode, cv::TermCriteria criteria, float windowFactor = 1.5f, bool verbose = true): 
		m_cMode(colorMode), m_wMode(weightingMode), m_criteria(criteria), FrameTracker(windowFactor, verbose) 
	{
		// TODO
	};

	virtual int init(const cv::Mat &frame, cv::Rect bb);

	virtual int update(const cv::Mat &frame, cv::Rect bb);

	virtual cv::Rect track(const cv::Mat &frame, cv::Rect prevBB);	

	int getInfo() { return 1; }	// not used

protected:
	// params
	int m_cMode, m_wMode;	// color and weighting mode
	cv::TermCriteria m_criteria;	// mean-shift iteration criteria
	// data
	cv::Mat m_bgModel;
	cv::Mat m_objModel;
};

#endif