#ifndef _OLMOT_EXPERIMENTER_H
#define _OLMOT_EXPERIMENTER_H

#include "../io/player.h"
#include "visualizer.h"
#include "manager.hpp"

template <typename TParam, typename TInfo>	// for parameters setting and visualization info, input is fixed as image frame
class MOTExperimenter: public CVPlayer
{
public:
	MOTExperimenter(): CVPlayer(false) {};	

	void setTrackingManager(cv::Ptr<TrackingManager<cv::Mat, TParam, TInfo>> mananger)
	{
		m_manager = mananger;
	}

	void setVisualizer(cv::Ptr<MOTVisualizer<TInfo>> visualizer)
	{
		m_visualizer = visualizer;
	}

	// override, the key function
	cv::Mat getFrameToShow(const cv::Mat &frame, int fIdx)
	{
		// process
		m_manager->process(frame, fIdx);

		// get visualization info
		TInfo vInfo;
		m_manager->getVisualization(vInfo);
		m_visualizer->setVisualInfo(vInfo);

		// visualize
		return m_visualizer->castOnFrame(frame, fIdx);
	}

	int getResult(TrackBasedResult &result)
	{
		return m_manager->getResult(result);
	}

private:
	cv::Ptr<TrackingManager<cv::Mat, TParam, TInfo>> m_manager;
	cv::Ptr<MOTVisualizer<TInfo>> m_visualizer;
};

#endif	// experimenter.h