#include "VanishPointDetector.h"

#include <opencv2/imgproc/imgproc.hpp>
#include "time.h"

using namespace std;
using namespace cv;

//#define VPD_MAX_NUM_LINES	200

int VanishPointDetector::processImage( const cv::Mat src, int numVps, bool drawOn /*= false*/, cv::Mat &outputImg /*= Mat()*/ )
{
	cv::Mat imgGray, imgCanny;
	if (src.channels() > 1)
	{
		cvtColor(src, imgGray, CV_BGR2GRAY);
		//cvtColor(src, imgGray, CV_RGB2GRAY);
	}
	else
		imgGray = src;

	clock_t sT = clock();

	// Canny
	cv::Canny(imgGray, imgCanny, m_canny1, m_canny2, m_canny3);
	bitwise_and(imgCanny, m_clearBoundaryMask, imgCanny);	
	//imshow("Canny", imgCanny);

	// Hough
	VPDLineSegments lineSegments;
	vector<cv::Point> aux;
	vector<cv::Vec4i> lines;	

	int houghThreshold = m_hough1;
	if(imgGray.cols * imgGray.rows < 400*400)
		houghThreshold = 100;			
	cv::HoughLinesP(imgCanny, lines, 1, CV_PI/180, houghThreshold, m_hough2, m_hough3);
	while(lines.size() > VPD_MAX_NUM_LINES)
	{
		lines.clear();
		houghThreshold += 10;
		cv::HoughLinesP(imgCanny, lines, 1, CV_PI/180, houghThreshold, m_hough2, m_hough3);
	}

	for(size_t i=0; i<lines.size(); i++)
	{		
		Point pt1, pt2;
		pt1.x = lines[i][0];
		pt1.y = lines[i][1];
		pt2.x = lines[i][2];
		pt2.y = lines[i][3];

		// Store into vector of pairs of Points for msac
		aux.clear();
		if (abs(pt1.x - pt2.x) > 1)	// filter vertical lines
		{
			aux.push_back(pt1);
			aux.push_back(pt2);
			lineSegments.push_back(aux);
		}		
	}

	// Multiple vanishing points
	std::vector<int> numInliers;

	if (m_verbose)
		cout << "#Line segs: " << lineSegments.size() << " \n";

	// Call msac function for multiple vanishing point estimation
	m_LSCluster.clear();
	m_vps.clear();
	m_msac.multipleVPEstimation(lineSegments, m_LSCluster, numInliers, m_vps, numVps); 

	if (m_verbose)
	{
		cout << "Detecting " << m_vps.size() <<" vanishing point(s) in " << (int)(clock() - sT) << " ms ...\n";
		//
		for(int v = 0; v < (int)m_vps.size(); v++)
		{
			cout << "VP " << v << "(" << m_vps[v].at<float>(0,0) << ", " << m_vps[v].at<float>(1,0) << ", " << m_vps[v].at<float>(2,0) << ")\n";
			double vpNorm = cv::norm(m_vps[v]);
			if(fabs(vpNorm - 1) < 0.001)
			{
				cout << "(INFINITE)\n";
			}
		}
	}
		
	// Draw line segments according to their cluster
	if (drawOn && src.channels() == 3)
	{
		src.copyTo(outputImg);
		m_msac.drawCS(outputImg, m_LSCluster, m_vps);
		// my draw: horizons
		vector<cv::Scalar> colors;
		colors.push_back(cv::Scalar(0,0,255)); // First is RED
		colors.push_back(cv::Scalar(0,255,0)); // Second is GREEN 
		colors.push_back(cv::Scalar(255,0,0)); // Third is BLUE
		for(int v = 0; v < (int)m_vps.size(); v++)
		{
			int tempY = (int)(m_vps[v].at<float>(1,0));
			line(outputImg, Point(0, tempY), Point(outputImg.cols - 1, tempY), colors[v], 2);
		}
	}	
	//
	return 1;
}