#include "detEvaluator.h"

#include "../utils.hpp"

#include <iostream>

using namespace std;
using namespace cv;

int DetectionEvaluator::generate( cv::Ptr<DetectionResult> dr, float recall, float prec, float shift )
{
	if (m_gt.empty())
	{
		cout << "No ground truth available. Please set ground truth first.\n";
		return 0;
	}

	dr->m_idx = m_gt->m_idx;

	int nF = (int)m_gt->m_result.size();
	int idx = m_gt->m_idx.sIdx,
		offset = m_gt->m_idx.offset;
	for (int i = 0; i < nF; i++)
	{
		MOTDetections dets = generateFrame(m_gt->m_result[i], recall, prec, m_size, shift);
		dr->m_result.push_back(dets);
		dr->m_fIdxes.push_back(idx);
		idx += offset;
	}
	
	dr->m_nFrames = nF;
	return 1;
}

MOTDetections DetectionEvaluator::generateFrame( MOTOutput &op, float recall, float prec, 
												cv::Size frameSize, float shift /*= 0.0f*/ )
{
	MOTDetections dets;
	int N = (int)op.size();
	float precThresh = recall / prec - recall;
	for (int i = 0; i < N; i++)
	{
		ObjFrameInfo &fInfo = op[i];
		BoundingBox bb = fInfo.info.bb;
		// for recall
		if (m_rng.uniform(0.0f, 1.0f) < recall)
		{
			if (0.0f != shift)
			{
				float sX = m_rng.uniform(-shift, shift),
					sY = m_rng.uniform(-shift, shift),
					sW = m_rng.uniform(-shift, shift),
					sH = m_rng.uniform(-shift, shift);
				BoundingBox tBB = cv::Rect(bb.x + (int)(sX * bb.width), bb.y + (int)(sY * bb.height),
					(int)(bb.width * (1.0f + sW)), (int)(bb.height * (1.0f + sH)));
				dets.push_back(Detection(checkRect(tBB, frameSize)));
			}
			else
				dets.push_back(Detection(bb));			
		}
		// for precision
		if (m_rng.uniform(0.0f, 1.0f) < precThresh)
		{
			// mirror
			BoundingBox tBB = cv::Rect(frameSize.width - bb.width - bb.x - 1, bb.y, bb.width, bb.height);
			dets.push_back(Detection(tBB));
		}
	}

	return dets;
}

DetEvalResult DetectionEvaluator::evaluate( cv::Ptr<DetectionResult> dr, double thresh /*= 0.5*/ )
{
	if (m_gt.empty())
	{
		cout << "No ground truth available. Please set ground truth first.\n";
		return DetEvalResult();
	}

	if (m_gt->m_result.size() != dr->m_result.size())
	{
		cout << "Ground truth and detection size not match.\n";
		cout << "GT: " << m_gt->m_result.size() << " Dets: " << dr->m_result.size();
		return DetEvalResult();
	}
	
	size_t i, n = m_gt->m_result.size();
	DetEvalResult res, temp;
	for (i = 0; i< n; i++)
	{
		evaluateFrame(dr->m_result[i], m_gt->m_result[i], temp);
		res.goodDets += temp.goodDets;
		res.falseAlarms += temp.falseAlarms;
		res.missDets += temp.missDets;
	}
	return res;
}

int DetectionEvaluator::evaluateFrame( const MOTDetections &dets, const MOTOutput &op,
											 DetEvalResult &res, double thresh /*= 0.5*/ )
{
	// clear
	res.goodDets = 0;
	//
	size_t i, j, nD = dets.size(), nO = op.size();
	cv::Mat score(nD, nO, CV_64FC1);
	for (i = 0; i < nD; i++)
	{
		BoundingBox dBB = dets[i].bb;
		for (j = 0; j < nO; j++)
		{
			BoundingBox oBB = op[j].info.bb;
			int inter = (oBB & dBB).area();
			score.at<double>(i, j) = (double)inter / (oBB.area() + dBB.area() - inter);	// VOC criteria
		}
	}
	//
	cv::Point maxLoc;
	double val;
	while (true)
	{
		minMaxLoc(score, NULL, &val, NULL, &maxLoc);
		// thresholding
		if (val <= thresh) break;

		res.goodDets++;
		// update for next minMaxLoc
		score.col(maxLoc.x) = cv::Scalar(thresh);
		score.row(maxLoc.y) = cv::Scalar(thresh);
	}
	//
	res.falseAlarms = nD - res.goodDets;
	res.missDets = nO - res.goodDets;
	//
	return 1;
}