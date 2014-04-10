#include "meanshifttracker.h"

#include "../core/basicrepr.h"
#include "../utils.hpp"

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int MeanShiftTracker::init( const cv::Mat &frame, cv::Rect bb )
{
	//
	Size s = frame.size();		
	m_window = checkRect(enlargeRect(bb, m_windowFactor, m_windowFactor), s);
	Mat hsv, objHist, bgHist, bg(frame, m_window);

	cvtColor(bg, hsv, CV_BGR2HSV);	// only need to convert the background
	Mat roi = hsv(bb - m_window.tl());	// cut from background

	int i, j, hBins = 18, sBins = 16;
	// the background histogram
	CommonHistograms::getHS2DHist(hsv, bgHist, hBins, sBins, false);	// do not normalized using NORM_L1
	// the target histogram
	CommonHistograms::getHS2DHist(roi, objHist, hBins, sBins, false);	// do not normalized using NORM_L1

	// get coefficients, and rescale objHist
	Mat mask = bgHist > 0;
	double minV;
	minMaxLoc(bgHist, &minV, 0, 0, 0, mask);
	float minVf = (float)minV;	// to avoid implicit conversion
	for(i = 0; i < hBins; i++)
	{
		for (j = 0; j < sBins; j++)
		{
			float v = bgHist.at<float>(i, j);
			if(v > 0)
				objHist.at<float>(i, j) *= minVf / v;
			// else the coefficient is 1.0f
		}
	}

	// normalize
	normalize(objHist, m_objModel, 0, 255, NORM_MINMAX);	

	return 1;
}

int MeanShiftTracker::update( const cv::Mat &frame, cv::Rect bb )
{
	// update is just the repetition of init
	// but can not determine when to update	

	return 1;
}

cv::Rect MeanShiftTracker::track( const cv::Mat &frame, cv::Rect prevBB )
{
	Size s = frame.size();		
	m_window = checkRect(enlargeRect(prevBB, m_windowFactor, m_windowFactor), s);
	Point anchor = m_window.tl();

	Mat hsv, backproj, roi(frame, m_window);
	cvtColor(roi, hsv, CV_BGR2HSV);

	int chs[] = {0, 1};	// HS
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
	calcBackProject(&hsv, 1, chs, m_objModel, backproj, ranges);

	if(m_verbose) imshow("BP", backproj);

	Rect bb = prevBB - anchor;
	meanShift(backproj, bb, m_criteria);

	return bb + anchor;
}