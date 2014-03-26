#ifndef _OLMOT_BASIC_SOT_HPP
#define _OLMOT_BASIC_SOT_HPP

#include "manager.hpp"
#include "visualizer.h"
#include "frametracker.h"

template <typename TInput, typename TOutput>
class SOTManager: public TrackingManager<cv::Mat, int, BoundingBox>
{
public:
	SOTManager(cv::Ptr<FrameTracker<TInput, TOutput>> tracker, BoundingBox bb, bool verbose = true): 
	  m_initBB(bb), m_tracker(tracker), TrackingManager(verbose) {};

	  // required
	  virtual int getVisualization(BoundingBox &vInfo)
	  {
		  vInfo = m_currentBB;
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

class SOTVisualizer: public MOTVisualizer<BoundingBox>
{
public:
	//SOTVisualizer(): MOTVisualizer<BoundingBox>() {};

	cv::Mat castOnFrame(const cv::Mat &img, int fIdx)
	{
		cv::Mat copy = img.clone();
		rectangle(copy, m_info, cv::Scalar(0, 0, 255), 2);
		return copy;
	}

	void setVisualInfo(const BoundingBox &bb)
	{
		m_info = bb;
	}
};

#endif	// basicsot.hpp

