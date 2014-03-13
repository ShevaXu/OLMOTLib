#ifndef _OLMOT_VISUALIZER_H
#define _OLMOT_VISUALIZER_H

#include "defs.h"

template <typename T>
class MOTVisualizer
{
public:
	MOTVisualizer() {};

	virtual cv::Mat castOnFrame(const cv::Mat &img, int fIdx) = 0;

	virtual void setVisualInfo(const T &info) = 0;

protected:
	T m_info;
};

//////////////////////////////////////////////////////////////////////////

struct DetGTInfo
{
	MOTOutput outputInfo;
	MOTDetections dets;
	DetEvalResult eval;
	bool showDetEval;
};

class ResultCaster: public MOTVisualizer<DetGTInfo>
{
public:
	ResultCaster(): m_showOutput(false), m_showDetections(false), m_showDetEval(false) 
	{
		// default
		m_outputColor = cv::Scalar(255, 0, 0);		
		m_detColor = cv::Scalar(0, 255, 0);
	};

	cv::Mat castOnFrame(const cv::Mat &img, int fIdx);

	virtual void setVisualInfo(const DetGTInfo &info)
	{
		m_info = info;
	}

private:
	bool m_showOutput, m_showDetections;
	bool m_showDetEval;
	cv::Scalar m_outputColor, m_detColor;
	DetGTInfo m_info;
};

#endif	// visualizer.h