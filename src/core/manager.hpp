#ifndef _OLMOT_MANAGER_H
#define _OLMOT_MANAGER_H

#include "defs.h"

//#include <iostream>

//#include <opencv2/imgproc/imgproc.hpp>

template <typename TInput, typename TParam, typename TInfo>	// for input, parameters setting and visualization info
class TrackingManager
{
public:
	TrackingManager(bool verbose = true): m_verbose(verbose), m_fIdx(-1), m_fCount(0)
	{
		// for timing
		m_freq = cv::getTickFrequency();
	}

	void setVerbose(bool verbose) { m_verbose = verbose; }
	void setParams(TParam &params) { m_params = params; }

	/************************************************************************/
	/*  The below methods follow the same setting:
		return -1 means pre-defined failure,
		and error is output immediately;
		return 1 for success                                                                   
	*/
	/************************************************************************/

	int process(const cv::Mat &frame)
	{
		return process(frame, m_fIdx + 1);
	}

	int process(const cv::Mat &frame, int fIdx)
	{
		if (m_verbose)
		{
			std::cout << "\n********************************\n"
				<< "******* Process frame # " << fIdx << " *******\n"
				<< "...\n";
		}

		// begin timing
		int64 bTime = cv::getTickCount();

		// fixed basic init
		if (fIdx == m_fIdx)
		{
			std::cout << "Error @Manager::init : same frame (index) should not be processed twice!\n";
			return -1;
		}
		m_fIdx = fIdx;
		m_fCount++;

		// other custom preparations
		if (-1 == prepare())
			return -1;

		// data association
		if (-1 == performDA())
			return -1;

		// infer states for trackers
		if (-1 == inferStates())
			return -1;

		// end timing
		int64 eTime = cv::getTickCount();

		if (m_verbose)
		{
			std::cout << "...\n"
				<< "******* Successfully process frame #" << fIdx << " in " << (eTime - bTime) / m_freq << " s! *******\n";
		}
		return 1;
	}

	// O methods
	virtual int getCurrentOutput(MOTOutput &output) = 0;
	virtual int getTrajectory(int id, MOTTrajectory &trajectory) = 0;
	virtual int getResult(TrackBasedResult &result) = 0;
	virtual int getVisualization(TInfo &vInfo) = 0;
	
protected:
	// actions to be specified
	virtual int prepare() = 0;
	virtual int performDA() = 0;	
	virtual int inferStates() = 0;	

protected:
	bool m_verbose;
	int m_fIdx,
		m_fCount;
	double m_freq;
	TParam m_params;

	// currently, only keep for two frames
	// might be more than image frames
	TInput m_currentInput,
		m_prevInput;
};

#endif // manager.h