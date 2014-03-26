#include "testUtils.h"

#include "../core/basicsot.hpp"

#include "../wrapper/pixeltrackwrapper.h"
#include "../wrapper/meanshifttracker.h"

using namespace std;
using namespace cv;

int testPixelTrack(std::string fileName, BoundingBox bb)
{
	cout << "Test for pixel track implementation!\n";

	MOTExperimenter<int, SOTInfo> me;

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	DatasetConfig dataset(baseDir + fileName);

	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);
	me.setReader(reader);

	//////////////////////////////////////////////////////////////////////////

	cv::Ptr<PixelTrackWrapper> tracker = new PixelTrackWrapper(false);
	cv::Ptr<TrackingManager<cv::Mat, int, SOTInfo>> manager = new SOTManager<PixelTrackFrameClips, double>(tracker, bb);

	cv::Ptr<MOTVisualizer<SOTInfo>> vis = new SOTVisualizer();

	me.setTrackingManager(manager);
	me.setVisualizer(vis);

	me.autoPlay();

	return 1;
}

int testMeanShift(std::string fileName, BoundingBox bb)
{
	cout << "Test for pixel track implementation!\n";

	MOTExperimenter<int, SOTInfo> me;

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	DatasetConfig dataset(baseDir + fileName);

	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);
	me.setReader(reader);

	//////////////////////////////////////////////////////////////////////////

	int mode = 0;
	cv::Ptr<MeanShiftTracker> tracker = new MeanShiftTracker(mode,
		TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), 1.5f);

	cv::Ptr<TrackingManager<cv::Mat, int, SOTInfo>> manager = new SOTManager<cv::Mat, int>(tracker, bb, true);

	cv::Ptr<MOTVisualizer<SOTInfo>> vis = new SOTVisualizer();

	me.setTrackingManager(manager);
	me.setVisualizer(vis);

	me.autoPlay();

	return 1;
}