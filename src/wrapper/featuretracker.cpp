#include "featuretracker.h"
#include "../utils.hpp"

#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/features2d.hpp>

using namespace cv;

FeatureTracker::FeatureTracker( float windowFactor /*= 1.5f*/, bool verbose /*= true*/ ): FrameTracker(windowFactor, verbose) 
{
	// default
	m_feat2D = new SURF();
	m_matcher = new BFMatcher(NORM_L1, true);

	/*SurfFeatureDetector detector( 400, 2, 2, false, true );
	SurfDescriptorExtractor extractor;
	BFMatcher matcher(NORM_L1, true);*/
};

int FeatureTracker::init( const cv::Mat &frame, cv::Rect bb )
{
	//
	Mat roi(frame, bb);
	m_feat2D->detect(roi, m_fromKPs);
	m_feat2D->compute(roi, m_fromKPs, m_fromDes);

	return 1;
}

int FeatureTracker::update( const cv::Mat &frame, cv::Rect bb )
{
	//
	Mat roi(frame, bb);
	m_feat2D->detect(roi, m_fromKPs);
	m_feat2D->compute(roi, m_fromKPs, m_fromDes);

	return 1;
}

cv::Rect FeatureTracker::track( const cv::Mat &frame, cv::Rect prevBB )
{
	Rect resBB = prevBB;

	Size s = frame.size();		
	m_window = checkRect(enlargeRect(prevBB, m_windowFactor, m_windowFactor), s);
	Point anchor = m_window.tl();

	Mat roi(frame, m_window);

	//
	m_feat2D->detect(roi, m_toKPs);
	m_feat2D->compute(roi, m_toKPs, m_toDes);

	// matching
	std::vector<DMatch> matches, good_matches;
	m_matcher->match( m_fromDes, m_toDes, matches );
	good_matches = filterGoodMatches(matches);

	int nGM = good_matches.size();
	if (nGM >= 3)
	{
		vector<Point2f> fromP, toP;
		for( int i = 0; i < nGM; i++ )
		{
			//-- Get the keypoints from the good matches
			fromP.push_back( m_fromKPs[ good_matches[i].queryIdx ].pt );
			toP.push_back( m_toKPs[ good_matches[i].trainIdx ].pt );
		}
		//
		m_trans = estimateRigidTransform( fromP, toP, true );
		//
		if (m_trans.data)
		{
			// use translation only
			resBB = Rect(Point(0, 0), prevBB.size())
				+ Point((int)m_trans.at<double>(0,2), (int)m_trans.at<double>(1,2))
				+ anchor;
			// or use full transform
		}		
	}

	return resBB;
}

std::vector<DMatch> FeatureTracker::filterGoodMatches( const std::vector<DMatch> &matches, 
													  int method /*= 0*/, double maxDist /*= 2.0*/, 
													  int nTimes /*= 3*/, double percentage /*= 0.4*/, int nDesire /*= 10*/ )
{
	vector<DMatch> goodMatches;
	int nM = matches.size(), i;
	if(0 == method)	// threshold
	{
		for( i = 0; i < nM; i++ )
		{ 
			if(matches[i].distance < maxDist)
				goodMatches.push_back(matches[i]);
		}
	}
	else if (1 == method)	// n times minDist
	{
		double max_dist = -1.0; double min_dist = 1000.0;
		for( i = 0; i < nM; i++ )
		{ 
			double dist = matches[i].distance;
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}
		/*printf("-- Max dist : %f \n", max_dist );
		printf("-- Min dist : %f \n", min_dist );*/
		//
		for( i = 0; i < nM; i++ )
		{ 
			if( matches[i].distance < nTimes * min_dist )
				goodMatches.push_back( matches[i]);
		}
	}
	return goodMatches;
}
