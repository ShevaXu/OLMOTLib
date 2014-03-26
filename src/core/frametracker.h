#ifndef _OLMOT_FRAME_TRACKER_H
#define _OLMOT_FRAME_TRACKER_H

#include <opencv2/core/core.hpp>

template <typename TInput, typename TOutput>	// for input frame info and optional output info
class FrameTracker
{
public:
	FrameTracker(float windowFactor = 1.5f, bool verbose = true): m_windowFactor(windowFactor), m_verbose(verbose) {};

	virtual int init(const TInput &frame, cv::Rect bb) = 0;

	virtual int update(const TInput &frame, cv::Rect bb) = 0;

	virtual cv::Rect track(const TInput &frame, cv::Rect prevBB) = 0;

	virtual TOutput getInfo() = 0;

	cv::Rect getSearchWindow() { return m_window; }

protected:
	float m_windowFactor;	// -1 for whole image
	bool m_verbose;
	// data
	cv::Rect m_window;
};

#endif	// frametracker.h