#include "testUtils.h"

#include "../core/basicsot.hpp"

#include "../wrapper/pixeltrackwrapper.h"

using namespace std;
using namespace cv;

int testPixelTrack(std::string fileName, BoundingBox bb)
{
	cout << "Test for pixel track implementation!\n";

	MOTExperimenter<int, BoundingBox> me;

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	DatasetConfig dataset(baseDir + fileName);

	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);
	me.setReader(reader);

	//////////////////////////////////////////////////////////////////////////

	cv::Ptr<PixelTrackWrapper> tracker = new PixelTrackWrapper(false);
	cv::Ptr<TrackingManager<cv::Mat, int, BoundingBox>> manager = new SOTManager<PixelTrackFrameClips, double>(tracker, bb);

	cv::Ptr<MOTVisualizer<BoundingBox>> vis = new SOTVisualizer();

	me.setTrackingManager(manager);
	me.setVisualizer(vis);

	me.autoPlay();

	return 1;
}