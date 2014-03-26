#include "pixeltrackwrapper.h"
#include "../utils.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int PixelTrackWrapper::init( const PixelTrackFrameClips &frame, cv::Rect bb )
{
	// check data
	if (!frame.bgr.data || !frame.xGrad.data || !frame.yGrad.data)
	{
		return -1;
	}

	Size s = frame.bgr.size();
	bb = checkRect(bb, s);

	if (m_useSeg)
	{
		//m_segMap = m_seg->segmentBGR(bgr, bb);
		m_model->learn(frame.bgr, frame.xGrad, frame.yGrad, bb, m_segMap);
	}
	else
		m_model->learn(frame.bgr, frame.xGrad, frame.yGrad, bb);	// when no segmentation

	return 1;
}

cv::Rect PixelTrackWrapper::track( const PixelTrackFrameClips &frame, cv::Rect prevBB )
{
	// check data
	if (!frame.bgr.data || !frame.xGrad.data || !frame.yGrad.data)
	{
		return Rect();
	}

	Size s = frame.bgr.size();		
	m_window = checkRect(enlargeRect(prevBB, m_windowFactor, m_windowFactor), s);

	Mat votingMap = Mat::zeros(s, CV_32FC1);
	m_model->vote(frame.bgr, frame.xGrad, frame.yGrad, m_window, votingMap);

	Point maxP;
	minMaxLoc(votingMap(m_window), 0, &m_votes, 0, &maxP);
	maxP = maxP + m_window.tl();
	Rect newBB = getRectFromCenterSize(maxP, prevBB.size());	// keep the size constant

	if (m_verbose)
	{
		cout << m_votes << '\t' << maxP << " <- " << m_window << endl;
	}

	return newBB;
}

int PixelTrackWrapper::update( const PixelTrackFrameClips &frame, cv::Rect bb )
{
	// check data
	if (!frame.bgr.data || !frame.xGrad.data || !frame.yGrad.data)
	{
		return -1;
	}

	if (m_useSeg)
	{
		//m_segMap = m_seg->segmentBGR(bgr, newBB);
		m_model->update(frame.bgr, frame.xGrad, frame.yGrad, bb, m_segMap);
	}
	else
		m_model->update(frame.bgr, frame.xGrad, frame.yGrad, bb);

	return 1;
}

PixelTrackFrameClips::PixelTrackFrameClips( cv::Mat _bgr )
{
	bgr = _bgr;
	cv::Mat gray;
	cvtColor(bgr, gray, CV_BGR2GRAY);
	
	Sobel(gray, xGrad, CV_16S, 1, 0, 3);
	Sobel(gray, yGrad, CV_16S, 0, 1, 3);
}