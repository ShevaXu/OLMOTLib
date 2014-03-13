#ifndef _OLMOT_DET_EVALUATOR_H
#define _OLMOT_DET_EVALUATOR_H

#include "../core/defs.h"

class DetectionEvaluator
{
public:
	DetectionEvaluator() 
	{
		resetRandomState();
	};

	DetectionEvaluator(cv::Ptr<FrameBasedGT> gt, cv::Size frameSize): m_gt(gt), m_size(frameSize)
	{
		resetRandomState();
	};

	void resetRandomState()
	{
		m_rng.state = (uint64)cv::getTickCount();
	}

	void setGroundTruth(cv::Ptr<FrameBasedGT> gt, cv::Size frameSize)
	{
		m_gt = gt;
		m_size = frameSize;
	}

	int generate(cv::Ptr<DetectionResult> dr, float recall, float prec, float shift);

	MOTDetections generateFrame(MOTOutput &op, float recall, float prec, cv::Size frameSize, float shift = 0.0f);

	DetEvalResult evaluate(cv::Ptr<DetectionResult> dr, double thresh = 0.5);

	static int evaluateFrame(const MOTDetections &dets, const MOTOutput &op, DetEvalResult &res, double thresh = 0.5);

private:
	cv::Ptr<FrameBasedGT> m_gt;
	cv::Size m_size;
	cv::RNG m_rng;
};

#endif	// detEvaluator.h