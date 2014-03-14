#ifndef _OLMOT_VANISH_POINT_DETECTOR_H
#define _OLMOT_VANISH_POINT_DETECTOR_H

#include "../../contribs/MSAC/MSAC.h"

#include <opencv2/core/core.hpp>

typedef std::vector<std::vector<cv::Point>> VPDLineSegments;
typedef std::vector<std::vector<std::vector<cv::Point> > > VPDLineSegmentsClusters;

class VanishPointDetector 
{
public:
	//enum vps_mode {VPS_MODE_IMAGE, VPS_MODE_VIDEO};
	static const int VPD_MAX_NUM_LINES = 200;

	VanishPointDetector(cv::Size frameSize, bool verbose = false, int mode = 0) : 
		m_procSize(frameSize), m_verbose(verbose), m_mode(mode)
	{
		m_msac.init(MODE_NIETO, frameSize, false);
		//
		m_clearBoundaryMask.create(frameSize, CV_8UC1);
		m_clearBoundaryMask.setTo(0);
		cv::Mat roi = m_clearBoundaryMask(cv::Rect(3, 3, frameSize.width -6, frameSize.height - 6));
		roi.setTo(255);
		//
		setCannyParams();
		setHoughParams();
	}
	
	void setCannyParams(int c1 = 200, int c2 = 150, int c3 = 3)
	{
		m_canny1 = c1;
		m_canny2 = c2;
		m_canny3 = c3;
	}

	void setHoughParams(int h1 = 70, int h2 = 20, int h3 = 2)
	{
		m_hough1 = h1;
		m_hough2 = h2;
		m_hough3 = h3;
	}

	void getResult(std::vector<cv::Mat> &vps, VPDLineSegmentsClusters &lsc)
	{
		vps = m_vps;
		lsc = m_LSCluster;
	}

	float getHorizon()
	{
		if (m_vps.size() > 0)
		{
			return m_vps[0].at<float>(1, 0) / m_procSize.height;
		}
		else
			return -1.0f;
	}

	cv::Point getVanishPoint()
	{
		if (m_vps.size() > 0)
		{
			return cv::Point((int)m_vps[0].at<float>(0, 0), (int)m_vps[0].at<float>(1, 0));
		}
		else
			return cv::Point();
	}

	//
	int processImage(const cv::Mat src, int numVPS, bool drawOn = false, cv::Mat &outputImg = cv::Mat());

private:
	cv::Size m_procSize;
	bool m_verbose;
	//
	int m_mode;	// not used so far

	MSAC m_msac;	

	VPDLineSegmentsClusters m_LSCluster;
	std::vector<cv::Mat> m_vps;

	cv::Mat m_clearBoundaryMask;

	// params
	int m_canny1,
		m_canny2,
		m_canny3;
	int m_hough1,
		m_hough2,
		m_hough3;
};

#endif	// VanishPointDetector.h