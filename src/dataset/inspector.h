#ifndef _OLMOT_INSPECTOR_H
#define _OLMOT_INSPECTOR_H

#include "../io/player.h"
#include "../core/visualizer.h"

#include "detEvaluator.h"

#include <iostream>

class MOTInspector: public CVPlayer
{
public:
	MOTInspector(): m_showEval(false), m_genDets(false),
		m_withGT(false), m_withDR(false), CVPlayer(false) 
	{
		m_caster = new ResultCaster;
		m_eval = new DetectionEvaluator;
	};

	// override
	cv::Mat getFrameToShow(const cv::Mat &frame, int fIdx)
	{
		DetGTInfo info;
		info.showDetEval = false;
		if (m_withGT && m_fCount < m_gt->m_nFrames)
		{
			info.outputInfo = m_gt->m_result[m_fCount];
			//
			if (m_genDets)
			{
				// the params are fixed for visualization
				MOTDetections dets = m_eval->generateFrame(m_gt->m_result[m_fCount], 0.8f, 0.8f, frame.size(), 0.1f);
				info.dets = dets;
				// if generate, must show evaluation result
				DetEvalResult res;
				m_eval->evaluateFrame(dets, m_gt->m_result[m_fCount], res);
				info.eval = res;
				info.showDetEval = true;
			}
		}
		if (m_withDR && m_fCount < m_dr->m_nFrames)
		{
			info.dets = m_dr->m_result[m_fCount];
			if (m_showEval)
			{
				DetEvalResult res;
				m_eval->evaluateFrame(m_dr->m_result[m_fCount], m_gt->m_result[m_fCount], res);
				info.eval = res;
				info.showDetEval = true;
			}
		}
		m_caster->setVisualInfo(info);
		//
		return m_caster->castOnFrame(frame, fIdx);
	}

	void setGT(cv::Ptr<FrameBasedGT> gt)
	{
		m_gt = gt;
		m_withGT = true;
	}

	void setDR(cv::Ptr<DetectionResult> dr)
	{
		m_dr = dr;
		m_withDR = true;
		m_genDets = false;
	}

	void showDetEval()
	{
		if (!m_withGT) return;

		if (!m_withDR) m_genDets = true;
		
		m_showEval = true;		
	}

private:
	bool m_showEval, m_genDets;
	bool m_withGT, m_withDR;
	cv::Ptr<ResultCaster> m_caster;
	cv::Ptr<FrameBasedGT> m_gt;	// only frame based
	cv::Ptr<DetectionResult> m_dr;
	cv::Ptr<DetectionEvaluator> m_eval;
};

#endif	// inspector.h