#include "testUtils.h"

#include "../core/experimenter.h"
#include "../core/basicrepr.h"

int testCoreTemplate()
{
	cv::Ptr<TrackingManager<cv::Mat, int ,DetGTInfo>> mm;
	MOTExperimenter<int, DetGTInfo> me;
	me.setTrackingManager(mm);
	cv::Ptr<MOTVisualizer<DetGTInfo>> mv = new ResultCaster;
	me.setVisualizer(mv);
	return 1;
}