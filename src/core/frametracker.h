#ifndef _OLMOT_FRAME_TRACKER_H
#define _OLMOT_FRAME_TRACKER_H

#include <opencv2/core/core.hpp>

template <typename TInput, typename TOutput>	// for input frame info and optional output info
class FrameTracker
{
public:
	FrameTracker(bool verbose = true): m_verbose(verbose) {};

	virtual int init(const TInput &frame, cv::Rect bb) = 0;

	virtual int update(const TInput &frame, cv::Rect bb) = 0;

	virtual cv::Rect track(const TInput &frame, cv::Rect prevBB) = 0;

	virtual TOutput getInfo() = 0;

protected:
	bool m_verbose;
};

#endif	// frametracker.h