#include "visualizer.h"
#include "../utils.hpp"

#include <sstream>
#include <iostream>

using namespace cv;
using namespace std;

cv::Mat ResultCaster::castOnFrame( const cv::Mat &img, int fIdx )
{
	bool showDetections = m_info.dets.size() > 0;
	bool showOutput = m_info.outputInfo.size() > 0;

	Mat copy;
	if (showDetections || showOutput)
	{
		copy = img.clone();
		// show frame index
		stringstream buffer;
		buffer << fIdx;
		string sIdx = buffer.str();				
		putText(copy, sIdx, Point(15, 35), 
			FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);

	}
	else return img;

	Size fSize = img.size();
	if (showDetections)
	{
		//
		int n = (int)m_info.dets.size();
		for (int i = 0; i < n; i++)
		{
			Rect tempR = m_info.dets[i].bb;
			tempR = checkRect(tempR, fSize);
			rectangle( copy, tempR, m_detColor, 1/*thickness*/ );

			if (m_info.dets[i].confidence != 0.0)
			{
				stringstream buffer;
				buffer << m_info.dets[i].confidence;
				string sConf = buffer.str();				
				putText(copy, sConf, Point(tempR.x - 5, tempR.y - 5), 
					FONT_HERSHEY_SIMPLEX, 0.5, m_detColor, 1);
			}			
		}
	}

	if (showOutput)
	{
		//
		int n = (int)m_info.outputInfo.size();
		for (int i = 0; i < n; i++)
		{
			Rect tempR = m_info.outputInfo[i].info.bb;
			tempR = checkRect(tempR, fSize);
			//rectangle( copy, tempR, m_outputColor, 2/*thickness*/ );
			Point center = (tempR.tl() + tempR.br()) * 0.5;
			Size axes(tempR.width / 2, tempR.height / 2);			
			ellipse(copy, center, axes, 0.0, 0.0, 360.0, m_outputColor, 2);

			stringstream buffer;
			buffer << m_info.outputInfo[i].id;
			string tid = buffer.str();				
			putText(copy, tid, Point(tempR.x - 5, tempR.y - 5), 
				FONT_HERSHEY_SIMPLEX, 1.0, m_outputColor, 2);
		}
	}

	if (m_info.showDetEval)
	{
		stringstream buffer;
		buffer << "Good: " << m_info.eval.goodDets << ' ';
		buffer << "FA: " << m_info.eval.falseAlarms << ' ';
		buffer << "Miss: " << m_info.eval.missDets;
		string tid = buffer.str();				
		putText(copy, tid, Point(15, 55), 
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2);
	}

	return copy;
}