#include "../io/player.h"
#include "../dataset/datasetconfig.h"

#include <iostream>

using namespace std;

void dataset2ImgSeq(const DatasetConfig &dataset, ImgSeqConfig &conf)
{
	conf.dir = dataset.m_dir;
	conf.sIdx = dataset.m_sIdx;
	conf.eIdx = dataset.m_eIdx;
	conf.offset = dataset.m_offset;
	conf.fileName = dataset.m_format;
	conf.frames = (conf.eIdx - conf.sIdx + 1) / conf.offset;
}

int testImgSeqPlayer(std::string datasetFile, std::string recordFile = "", int fps = 20)
{
	cout << "Test for image sequence player!\n";

	std::string baseDir = "D:\\tracking-dataset\\configs\\";	

	CVPlayer player(false);
	DatasetConfig dataset(baseDir + datasetFile);
	ImgSeqConfig conf;
	dataset2ImgSeq(dataset, conf);

	cv::Ptr<ImgSeqReader> reader = new ImgSeqReader(conf);

	player.setReader(reader);
	if (recordFile != "")
	{
		std::string outUrl = "output\\" + recordFile;
		player.setRecordFile(outUrl, fps);
	}
	
	player.autoPlay();

	return 1;
}

int testVidPlayer(std::string vidFile)
{
	cout << "Test for video player!\n";	

	CVPlayer player(false);
	cv::Ptr<VideoReader> reader = new VideoReader(vidFile);
	player.setReader(reader);
	player.autoPlay();

	return 1;
}