#include "forebackgroundmodel.h"

#include "../core/basicrepr.h"
#include "../utils.hpp"

#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void PixelSegmentation::computeFGBG(const cv::Mat &hsv, cv::Rect bb)
{
	Size s = hsv.size();
	Mat fgImg = hsv(checkRect(enlargeRect(bb, m_scaleXFG, m_scaleYFG), s));
	Mat bgImg = hsv(checkRect(enlargeRect(bb, m_scaleXBG, m_scaleYBG), s));

	// get histogram, do not normalize here
	CommonHistograms::getHS2DHist(fgImg, m_fgHist, m_hBins, m_sBins, false);
	CommonHistograms::getHS2DHist(bgImg, m_bgHist, m_hBins, m_sBins, false);
	
	// get actual background hist
	m_bgHist = m_bgHist - m_fgHist;

	// normalize here
	normalize(m_fgHist, m_fgHist, 1.0, 0, cv::NORM_L1, -1, Mat());
	normalize(m_bgHist, m_bgHist, 1.0, 0, cv::NORM_L1, -1, Mat());
}

int PixelSegmentation::segment(const cv::Mat &hsv, cv::Rect bb, cv::Mat &seg)
{
	if (!hsv.data) return 0;

	computeFGBG(hsv, bb);
	//
	int i, j, li = bb.br().y, lj = bb.br().x;
	int divH = 180 / m_hBins,
		divS = 256 / m_sBins;
	for (i = bb.y; i < li; i++)
	{
		for (j = bb.x; j < lj; j++)
		{
			cv::Vec3b v = hsv.at<cv::Vec3b>(i, j);
			int h = v[0] / divH,
				s = v[1] / divS;
			float fgProb = m_fgHist.at<float>(h, s);
			seg.at<float>(i, j) = fgProb / (fgProb + m_bgHist.at<float>(h, s));
		}
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

DefaultBgSubtractor::DefaultBgSubtractor( int modelSelection )
{
	//if (modelSelection < 1) m_subtractor = createBackgroundSubtractorMOG();	// for OpenCV 3.0

	// use default params
	if (modelSelection < 1) m_subtractor = new BackgroundSubtractorMOG();			
	if (modelSelection == 1) m_subtractor = new BackgroundSubtractorMOG2();
	// set initializationFrames to 10 for quick initialization, only possible for OpenCV 3.0, current default 120
	if (modelSelection > 1) m_subtractor = new BackgroundSubtractorGMG();
}

void DefaultBgSubtractor::init( cv::InputArray frame )
{
	Mat dummy;	// the foreground mask during init is useless
	m_subtractor->operator ()(frame, dummy);
	//m_subtractor->apply(frame, dummy);	// for OpenCV 3.0
}

void DefaultBgSubtractor::apply( cv::InputArray frame, cv::OutputArray fgMask )
{
	m_subtractor->operator ()(frame, fgMask);
	//m_subtractor->apply(frame, fgMask);	// for OpenCV 3.0
}

void DefaultBgSubtractor::getContours( cv::InputArray fgMask, cv::OutputArrayOfArrays contours )
{
	Mat fg = fgMask.getMat().clone();

	// TODO: further params setting
	erode(fg, fg, Mat());
	dilate(fg, fg, Mat());
	findContours(fg, contours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
}
