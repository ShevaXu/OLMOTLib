#ifndef _OLMOT_MEAN_SHIFT_TRACKER_H
#define _OLMOT_MEAN_SHIFT_TRACKER_H

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../core/frametracker.h"
#include "../utils.hpp"


class MeanShiftTracker: public FrameTracker<cv::Mat, int>
{
public:
	MeanShiftTracker(int mode, cv::TermCriteria criteria, float windowFactor = 1.5f, bool verbose = true): 
		m_mode(mode), m_criteria(criteria), FrameTracker(windowFactor, verbose) 
	{
		m_mode = 0;	// since CamShift's performance is bad, just use meanShift
	};

	virtual int init(const cv::Mat &frame, cv::Rect bb)
	{
		//
		cv::Mat hsv, hue, hist, roi(frame, bb);
		cvtColor(roi, hsv, CV_BGR2HSV);
		
		/*inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
			Scalar(180, 256, MAX(_vmin, _vmax)), mask);*/

		int ch[] = {0, 0};
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);

		int hsize = 16;
		float hranges[] = {0,180};
		const float* phranges = hranges;
		calcHist(&hue, 1, 0, cv::Mat(), hist, 1, &hsize, &phranges);
		normalize(hist, m_histModel, 0, 255, CV_MINMAX);

		return 1;
	}

	virtual int update(const cv::Mat &frame, cv::Rect bb) { return 1; };

	virtual cv::Rect track(const cv::Mat &frame, cv::Rect prevBB)
	{
		cv::Size s = frame.size();		
		m_window = checkRect(enlargeRect(prevBB, m_windowFactor, m_windowFactor), s);
		cv::Point anchor = m_window.tl();

		cv::Mat hsv, hue, backproj, roi(frame, m_window);
		cvtColor(roi, hsv, CV_BGR2HSV);
		
		/*inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
			Scalar(180, 256, MAX(_vmin, _vmax)), mask);*/

		int ch[] = {0, 0};
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);

		int hsize = 18;
		float hranges[] = {0,180};
		const float* phranges = hranges;
		calcBackProject(&hue, 1, 0, m_histModel, backproj, &phranges);

		//imshow("BP", backproj);

		cv::Rect bb = prevBB - anchor;
		if (0 == m_mode)	// meanShift
		{
			meanShift(backproj, bb, m_criteria);
		}
		else
		{
			cv::RotatedRect trackBox = CamShift(backproj, bb, m_criteria);
			bb = trackBox.boundingRect();
		}

		return bb + anchor;
	}

	int getInfo() { return 1; }	// not used

protected:
	int m_mode;
	cv::TermCriteria m_criteria;
	cv::Mat m_histModel;
};

#endif