#ifndef _OLMOT_INSPECTOR_H
#define _OLMOT_INSPECTOR_H

#include "../io/player.h"
#include "../core/visualizer.h"

//#include "deteval.h"
//#include "detgenerator.h"

#include <iostream>

class MOTInspector: public CVPlayer
{
public:
	MOTInspector(bool showDetEval, bool showGenDets = true): m_showEval(showDetEval), m_showGenDets(showGenDets),
		m_withGT(false), m_withDR(false), CVPlayer(false) 
	{
		m_caster = new ResultCaster;
		/*if (showDetEval)
		{
			m_eval = new DetectionEvaluator();
		}*/
	};

	// override
	cv::Mat getFrameToShow(const cv::Mat &frame, int fIdx)
	{
		if (m_withGT && m_fCount < m_gt->m_nFrames)
		{
			m_caster->setVOutput(m_gt->m_result[m_fCount]);	// fCount is increased after this
		}
		if (m_withDR && m_fCount < m_dr->m_nFrames)
		{
			m_caster->setVDet(m_dr->m_result[m_fCount]);	// fCount is increased after this
		}
		/*if (m_showEval && m_withGT && m_fCount < m_gt->m_nFrames && m_withDR && m_fCount < m_dr->m_nFrames)
		{
			DetEvalResult res;
			m_eval->evaluateFrame(m_dr->m_result[m_fCount], m_gt->m_result[m_fCount], res);
			std::cout << "Good dets: " << res.m_gtDets << " false alarms: " << res.m_falseAlarms <<
				" missing: " << res.m_missDets << std::endl;
		}
		if (m_withGT && m_fCount < m_gt->m_nFrames && !m_withDR && m_showGenDets)
		{
			if (m_gen.empty())
			{
				m_gen = new DetectionGenerator(m_gt, frame.size());
			}
			MOTDetections dets = m_gen->generateFrame(m_gt->m_result[m_fCount], 0.7f, 0.7f, 0.0f);
			m_caster->setVDet(dets);
			if (m_showEval)
			{
				DetEvalResult res;
				m_eval->evaluateFrame(dets, m_gt->m_result[m_fCount], res);
				std::cout << "Good dets: " << res.m_gtDets << " false alarms: " << res.m_falseAlarms <<
					" missing: " << res.m_missDets << std::endl;
			}
		}*/
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
	}

private:
	bool m_showEval, m_showGenDets;
	bool m_withGT, m_withDR;
	cv::Ptr<ResultCaster> m_caster;
	cv::Ptr<FrameBasedGT> m_gt;	// only frame based
	cv::Ptr<DetectionResult> m_dr;
	/*cv::Ptr<DetectionEvaluator> m_eval;
	cv::Ptr<DetectionGenerator> m_gen;*/
};

#endif	// inspector.h