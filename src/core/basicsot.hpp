#ifndef _OLMOT_BASIC_SOT_HPP
#define _OLMOT_BASIC_SOT_HPP

#include "manager.hpp"
#include "visualizer.h"
#include "frametracker.h"

struct SOTInfo
{
	BoundingBox window, bb;
};

//////////////////////////////////////////////////////////////////////////

template <typename TInput, typename TOutput>
class SOTManager: public TrackingManager<cv::Mat, int, SOTInfo>
{
public:
	SOTManager(cv::Ptr<FrameTracker<TInput, TOutput>> tracker, BoundingBox bb, bool verbose = true): 
	  m_initBB(bb), m_tracker(tracker), TrackingManager(verbose) {};

	  // required
	  virtual int getVisualization(SOTInfo &vInfo)
	  {
		  vInfo.bb = m_currentBB;
		  vInfo.window = m_tracker->getSearchWindow();
		  return 1;
	  }

protected:
	// actions to be specified
	virtual int prepare()
	{
		TInput frame(m_currentInput);	// convert to tracker input
		m_frame = frame;
		return 1;
	}

	virtual int performDA() { return 1; };	// no need

	virtual int inferStates()
	{
		if (m_fCount <= 1)	// initialization
		{
			m_tracker->init(m_frame, m_initBB);
			m_currentBB = m_initBB;
		}
		else	// tracking
		{
			m_currentBB = m_tracker->track(m_frame, m_currentBB);
		}
		return 1;
	}

	virtual int update()
	{
		if (m_fCount > 1)
			m_tracker->update(m_frame, m_currentBB);
		return 1;
	}

private:
	BoundingBox m_initBB;
	cv::Ptr<FrameTracker<TInput, TOutput>> m_tracker;
	// data
	TInput m_frame;
	BoundingBox m_currentBB;
};

//////////////////////////////////////////////////////////////////////////

class SOTVisualizer: public MOTVisualizer<SOTInfo>
{
public:
	//SOTVisualizer(): MOTVisualizer<BoundingBox>() {};

	cv::Mat castOnFrame(const cv::Mat &img, int fIdx)
	{
		cv::Mat copy = img.clone();
		rectangle(copy, m_info.bb, cv::Scalar(0, 0, 255), 2);
		rectangle(copy, m_info.window, cv::Scalar(255, 0, 0), 2);
		return copy;
	}

	void setVisualInfo(const SOTInfo &info)
	{
		m_info = info;
	}
};

#endif	// basicsot.hpp

