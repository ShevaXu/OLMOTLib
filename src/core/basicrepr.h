#ifndef _OLMOT_BASIC_REPR_H
#define _OLMOT_BASIC_REPR_H

#include "defs.h"

//////////////////////////////////////////////////////////////////////////
// custom OpenCV histogram computation
class CommonHistograms
{
public:
	//static const int MOT_HIST_HS = 0;
	static int getBGRConcatHist(const cv::Mat &bgr, cv::Mat &hist, int bins, bool normalized = true);

	// 2 dims histogram on HSV space, but use only hue and saturation
	static int getHS2DHist(const cv::Mat &hsv, cv::Mat &hist, int hBins, int sBins, bool normalized = true);

};

//////////////////////////////////////////////////////////////////////////

class ColorHistRepr: public Representation
{
public:
	ColorHistRepr(const cv::Mat &patch, int bins);	

	// implemented
	double compare(cv::Ptr<Representation> other, int mode);	
	double compare(cv::Ptr<Representation> other);

	cv::Mat toMat()
	{
		return m_featureM;
	}

	std::string toString()
	{
		return "Color (RGB) histogram!\n";
	}

private:
	cv::Mat m_featureM;
};

//////////////////////////////////////////////////////////////////////////
// a affinity comparison class that provide static methods to compare primitive elements
class AffinityCmp
{
public:
	static const int MOT_RECT_F1 = 0;
	static const int MOT_RECT_GAUSSIAN = 0;

	static double vonMises(cv::Point v1, cv::Point v2, double sigma = 4.0);
	static double vonMisesGP(cv::Point3f v1, cv::Point3f v2, double sigma = 4.0);
	static double rectSim(const cv::Rect &r1, const cv::Rect &r2, int compMethod);
	static double sizeAffinity(const cv::Rect &r1, const cv::Rect &r2);
	static double scaleAffinity(const cv::Rect &r1, const cv::Rect &r2);

};


#endif	// basicrepr.h





