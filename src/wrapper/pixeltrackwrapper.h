#ifndef _OLMOT_PIXELTRACK_WRAPPER_H
#define _OLMOT_PIXELTRACK_WRAPPER_H

#include "../core/frametracker.h"

#include "../../contribs/PixelTrack/HSVPixelGradientModel.h"
//#include "pixelSeg.hpp"

// TODO: made the param available for setting

struct PixelTrackFrameClips
{
	PixelTrackFrameClips() {};

	PixelTrackFrameClips(cv::Mat bgr);

	cv::Mat bgr, xGrad, yGrad;	// required, pass xGrad and yGrad to avoid re-computation Sobel
};

class PixelTrackWrapper: public FrameTracker<PixelTrackFrameClips, double>
{
public:
	PixelTrackWrapper(bool useSegmentation, float windowFactor = 1.5f): 
		m_useSeg(useSegmentation), m_windowFactor(windowFactor)
	{
		m_model = new HSVPixelGradientModel(16, 16, 8, 1, 60);
		/*if (m_useSeg)
		{
			m_seg = new PixelSegmentation(10, 16);
		}*/
	};

	int init(const PixelTrackFrameClips &frame, cv::Rect bb);

	int update(const PixelTrackFrameClips &frame, cv::Rect bb);

	void setUseSeg(bool useSeg)
	{
		m_useSeg = useSeg;
	}

	cv::Rect track(const PixelTrackFrameClips &frame, cv::Rect prevBB);

	//double getVotes() { return m_votes; }
	double getInfo() { return m_votes; }

	//cv::Mat getSegmentation() { return m_segMap; }

private:
	// flags
	bool m_useSeg;
	// params
	float m_windowFactor;
	// for access
	double m_votes;
	cv::Mat m_segMap;
	cv::Rect m_window;
	// the model
	cv::Ptr<HSVPixelGradientModel> m_model;
	//cv::Ptr<PixelSegmentation> m_seg;

};

#endif	// pixeltrackwrapper.h

