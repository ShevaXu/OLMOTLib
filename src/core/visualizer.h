#ifndef _OLMOT_VISUALIZER_H
#define _OLMOT_VISUALIZER_H

#include "defs.h"

class MOTVisualizer
{
public:
	MOTVisualizer() {};

	virtual cv::Mat castOnFrame(const cv::Mat &img, int fIdx) = 0;

};

class ResultCaster: MOTVisualizer
{
public:
	ResultCaster(): m_showOutput(false), m_showDetections(false), m_showDetEval(false) 
	{
		// default
		m_outputColor = cv::Scalar(255, 0, 0);		
		m_detColor = cv::Scalar(0, 255, 0);
	};

	cv::Mat castOnFrame(const cv::Mat &img, int fIdx);	

	void setVOutput(const MOTOutput &outputInfo)
	{
		m_outputInfo = outputInfo;
		m_showOutput = true;
	}

	void setVDet(const MOTDetections &dets)
	{
		m_dets = dets;
		m_showDetections = true;
	}

	void setDetEval(DetEvalResult result)
	{
		m_showDetEval = true;
		m_result = result;
	}

private:
	bool m_showOutput, m_showDetections;
	bool m_showDetEval;
	cv::Scalar m_outputColor, m_detColor;
	MOTOutput m_outputInfo;
	MOTDetections m_dets;
	DetEvalResult m_result;
};

#endif	// visualizer.h