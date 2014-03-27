#include "basicrepr.h"

#include "../utils.hpp"

#include <opencv2/imgproc/imgproc.hpp>

// to disable warning in contributing code
// should remove when publishing for cross-platform
#pragma warning (push, 0)
#include "../../contribs/PROB/prob.hpp"
#pragma warning (pop)

using namespace cv;

ColorHistRepr::ColorHistRepr( const Mat &patch, int bins )
{
	if( patch.data )
	{ 
		//Mat hist_c1, hist_c2, hist_c3, featureM;
		//// Separate the image in 3 places ( B, G and R )
		//vector<Mat> bgr_planes;
		//split( patch, bgr_planes );
		//int histSize = bins;
		//// Set the ranges ( for B,G,R) )
		//float range[] = { 0, 256 } ;
		//const float* histRange = { range };
		//// Compute the histograms:
		//calcHist( &bgr_planes[0], 1, 0, Mat(), hist_c1, 1, &histSize, &histRange);
		//calcHist( &bgr_planes[1], 1, 0, Mat(), hist_c2, 1, &histSize, &histRange);
		//calcHist( &bgr_planes[2], 1, 0, Mat(), hist_c3, 1, &histSize, &histRange);
		//// concencate hist
		//m_featureM.create(histSize * 3, 1, hist_c1.type());
		//hist_c1.copyTo(m_featureM(Range(0, histSize), Range::all()));
		//hist_c2.copyTo(m_featureM(Range(histSize, histSize * 2), Range::all()));
		//hist_c3.copyTo(m_featureM(Range(histSize * 2, histSize * 3), Range::all()));
		//// normalize here instead
		//normalize(m_featureM, m_featureM, 1.0, 0, NORM_L1, -1, Mat() );	

		CommonHistograms::getBGRConcatHist(patch, m_featureM, bins, true);
	}
	else
		m_featureM = Mat();
}

double ColorHistRepr::compare( cv::Ptr<Representation> other, int mode )
{
	if (other.empty()) return 0.0;
	// require same size and type to compare
	Mat hist = other->toMat();
	assert(m_featureM.size() == hist.size());
	assert(m_featureM.type() == hist.type());
	// histogram comparison
	double score = compareHist(m_featureM, hist, mode); 
	return score;
}

double ColorHistRepr::compare( cv::Ptr<Representation> other )
{
	double score = compare(other, CV_COMP_HELLINGER);	// raw Hellinger score, the smaller the more similar
	score = std::exp(-score);	// rescale to 0~1
	return score;
}

//////////////////////////////////////////////////////////////////////////

double AffinityCmp::vonMises( cv::Point v1, cv::Point v2, double sigma /*= 4.0*/ )
{
	// use von_mises pdf
	double cosv = (v1.x * v2.x + v1.y * v2.y) / norm(v1) / norm(v2);
	double angle = std::acos(cosv);
	// 
	return von_mises_pdf(angle, 0.0, sigma);
}

double AffinityCmp::vonMisesGP( cv::Point3f v1, cv::Point3f v2, double sigma /*= 4.0*/ )
{
	// use von_mises pdf
	double cosv = (v1.x * v2.x + v1.z * v2.z) / norm(v1) / norm(v2);
	double angle = std::acos(cosv);
	// 
	return von_mises_pdf(angle, 0.0, sigma);
}

double AffinityCmp::rectSim( const Rect &r1, const Rect &r2, int compMethod )
{
	double score = 0.0;
	if (compMethod == MOT_RECT_F1)
	{
		cv::Rect inter = r1 & r2;
		// if overlapped
		if (inter.area() > 0)
		{
			double r = (double)inter.area() / (double)r1.area();
			double p = (double)inter.area() / (double)r2.area();
			score += 2 * r * p / (r + p);	// F1 style score
		}
	}
	else if (compMethod == MOT_RECT_GAUSSIAN)
	{
		// r1 is the base
		Point c1 = getCenterFromRect(r1),
			c2 = getCenterFromRect(r2);
		score = 1.0;
		//score *= getGaussianP((double)(c1.x - c2.x), (double)r1.width / 5.0);	// x
		//score *= getGaussianP((double)(c1.y - c2.y), (double)r1.height / 5.0);	// y
		//score *= getGaussianP((double)(r1.width - r2.width), 5.0);	// width
		//score *= getGaussianP((double)(r1.height - r2.height), 5.0);	// height				
		score *= getNegExp((double)(c1.x - c2.x), (double)r1.width);	// x
		score *= getNegExp((double)(c1.y - c2.y), (double)r1.height);	// y
		score *= getNegExp((double)(r1.width - r2.width), (double)r1.width);	// width
		score *= getNegExp((double)(r1.height - r2.height), (double)r1.height);	// height
	}
	return score;
}

double AffinityCmp::sizeAffinity( const Rect &r1, const Rect &r2 )
{
	double score = 1.0;
	score *= getNegExp((double)(r1.width - r2.width), (double)r1.width);	// width
	score *= getNegExp((double)(r1.height - r2.height), (double)r1.height);	// height
	return score;
}

double AffinityCmp::scaleAffinity( const Rect &r1, const Rect &r2 )
{
	double score = 1.0;
	double w1 = r1.width, w2 = r2.width,
		h1 = r1.height, h2 = r2.height;
	score *= w1 > w2 ? (w2 / w1) : (w1 / w2);
	score *= h1 > h2 ? (h2 / h1) : (h1 / h2);
	return score;
}

//////////////////////////////////////////////////////////////////////////

int CommonHistograms::getBGRConcatHist( const cv::Mat &bgr, cv::Mat &hist, int bins, bool normalized /*= true*/ )
{
	if (!bgr.data) return 0;

	Mat hist_c1, hist_c2, hist_c3;
	int histSize = bins;	
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split( bgr, bgr_planes );
	
	// Compute the histograms for each channel
	calcHist( &bgr_planes[0], 1, 0, Mat(), hist_c1, 1, &histSize, &histRange);
	calcHist( &bgr_planes[1], 1, 0, Mat(), hist_c2, 1, &histSize, &histRange);
	calcHist( &bgr_planes[2], 1, 0, Mat(), hist_c3, 1, &histSize, &histRange);

	// concencate hist
	hist.create(histSize * 3, 1, hist_c1.type());
	hist_c1.copyTo(hist(Range(0, histSize), Range::all()));
	hist_c2.copyTo(hist(Range(histSize, histSize * 2), Range::all()));
	hist_c3.copyTo(hist(Range(histSize * 2, histSize * 3), Range::all()));

	if (normalized)
	{
		normalize(hist, hist, 1.0, 0, NORM_L1, -1, Mat() );	
	}

	return 1;
}

int CommonHistograms::getHS2DHist( const cv::Mat &hsv, cv::Mat &hist, int hBins, int sBins, bool normalized /*= true*/ )
{
	if (!hsv.data) return 0;

	int histSize[] = { hBins, sBins };
	// hue varies from 0 to 360, but half down to 0-180
	// saturation [0, 1] scale to [0, 255]
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
	// Use the 0-th and 1-st channels
	int channels[] = { 0, 1 };

	// Calculate the histograms for the HSV images
	cv::calcHist( &hsv, 1, channels, Mat(), hist, 2, histSize, ranges);

	if (normalized)
	{
		normalize(hist, hist, 1.0, 0, NORM_L1, -1, Mat() );	
	}

	return 1;
}