#ifndef _OLMOT_FORE_BACKGROUND_MODEL_H
#define _OLMOT_FORE_BACKGROUND_MODEL_H

#include <opencv2/core/core.hpp>

//////////////////////////////////////////////////////////////////////////
// pixel-wise independent segmentation
// adopted from Duffner's PixelTrack, ICCV 2013
// no fb/bg accumulation for now
class PixelSegmentation
{
public:
	PixelSegmentation(int hBins, int sBins): 
		m_hBins(hBins), m_sBins(sBins),
		m_scaleXFG(1.0f), m_scaleYFG(1.0f), m_scaleXBG(1.5f), m_scaleYBG(1.5f)
	{
		// TODO
	};

	void setScaleFactor(float xFG, float yFG, float xBG, float yBG)
	{
		if (xFG < xBG)
		{
			m_scaleXFG = xFG;
			m_scaleXBG = xBG;
		}
		if (yFG < yBG)
		{
			m_scaleYFG = yFG;
			m_scaleYBG = yBG;
		}
	}

	// the seg image must have the same size as hsv, and be CV_32FC1
	int segment(const cv::Mat &hsv, cv::Rect bb, cv::Mat &seg);

protected:
	void computeFGBG(const cv::Mat &hsv, cv::Rect bb);

protected:
	// params
	int m_hBins, 
		m_sBins;
	float m_scaleXFG, m_scaleYFG, m_scaleXBG, m_scaleYBG;
	// data
	cv::Mat m_fgHist, m_bgHist;
	
};

//////////////////////////////////////////////////////////////////////////
// the conceptual dynamic background model for static scenes
// utilize the object detections results
class DynamicBackgroundModel
{
public:
	DynamicBackgroundModel() {};

protected:
	cv::Mat m_background;	// the background image, CV_8UC3, BGR image
	cv::Mat m_confMap;	// CV_32FC1, same size as image, background confidence
};

#endif	// forebackgroundmodel.h