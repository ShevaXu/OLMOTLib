#include "testUtils.h"
#include "..\dataset\inspector.h"
#include "..\dataset\xmlfilewrapper.h"

int testSampleConfig(std::string fileName)
{
	DatasetConfig::writeOutSampleXml("output\\" + fileName);
	return 1;
}

int testInspector(std::string fileName)
{
	cout << "Test for inspector!\n";

	MOTInspector inspector(true);

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	DatasetConfig dataset(baseDir + fileName);
	
	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);
	inspector.setReader(reader);

	// ground truth
	cv::Ptr<FrameBasedGT> fbGT = new FrameBasedGT(dataset.m_fbGTUrl);
	XMLFileWrapper::load(*fbGT);
	
	inspector.setGT(fbGT);

	// detection
	if (dataset.m_detResUrls.size() > 0)
	{
		cv::Ptr<DetectionResult> dr = new DetectionResult(dataset.m_detResUrls[0]);	
		XMLFileWrapper::load(*dr);
		inspector.setDR(dr);
	}	

	inspector.autoPlay();

	return 1;
}

int testConvertedGT(std::string fileName)
{
	cout << "Test for GT conversion!\n";

	MOTInspector inspector(true);

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	DatasetConfig dataset(baseDir + fileName);
	
	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);
	inspector.setReader(reader);

	// ground truth
	cv::Ptr<TrackBasedGT> tbGT = new TrackBasedGT(dataset.m_tbGTUrl);
	XMLFileWrapper::load(*tbGT);
	cv::Ptr<FrameBasedGT> fbGT = new FrameBasedGT(dataset.m_fbGTUrl);
	XMLFileWrapper::convert(*tbGT, *fbGT);
	
	inspector.setGT(fbGT);	

	inspector.autoPlay();

	return 1;
}