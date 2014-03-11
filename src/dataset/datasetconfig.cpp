#include "datasetconfig.h"

#include <iostream>

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

void DatasetConfig::writeOutSampleXml( std::string sampleURL )
{
	FileStorage fs(sampleURL, FileStorage::WRITE);
	fs << "name" << "PET09";
	fs << "dir" << "D:\\tracking-dataset\\PETS 2009\\S2_L1_View001\\View_001";
	fs << "format" << "frame_####.jpg";
	fs << "FrameWidth" << 640;
	fs << "FrameHeight" << 480;
	fs << "sIdx" << 0;
	fs << "eIdx" << 794;
	fs << "offset" << 1;
	fs << "fbGTUrl" << "D:\\tracking-dataset\\PETS 2009\\S2_L1_View001\\PETS2009-S2L1.xml";
	fs << "tbGTUrl" << "D:\\tracking-dataset\\PETS 2009\\S2_L1_View001\\PETS09_View001_S2_L1_000to794.avi.gt.xml";
	std::vector<string> drstrs;
	drstrs.push_back("D:\\tracking-dataset\\PETS 2009\\S2_L1_View001\\PETS09_View001_S2_L1_000to794.avi.detection.xml");
	drstrs.push_back("D:\\tracking-dataset\\Andriyenko's detection results\\PETS2009-S2L1-c1-det.xml");
	drstrs.push_back("D:\\tracking-dataset\\MyRawDetections\\PET09_C4_RawDet.xml");
	drstrs.push_back("D:\\tracking-dataset\\MyRawDetections\\PET09_C4_Responses.xml");
	fs << "detResUrls" << drstrs;
	fs.release();
}

bool DatasetConfig::loadSetting( std::string fileName )
{
	FileStorage fs(fileName, FileStorage::READ);
	if (!fs.isOpened())
	{
		cout << "Fail to load param.xml!\n";
		return false;
	}
	//
	m_datasetName = fs["name"];
	m_dir = fs["dir"];
	m_format = fs["format"];
	//
	m_sIdx = (int)fs["sIdx"];
	m_eIdx = (int)fs["eIdx"];
	m_offset = (int)fs["offset"];
	//
	m_fWidth = (int)fs["FrameWidth"];
	m_fHeight = (int)fs["FrameHeight"];
	//
	m_fbGTUrl = fs["fbGTUrl"];
	m_tbGTUrl = fs["tbGTUrl"];
	fs["detResUrls"] >> m_detResUrls;
	//
	m_setting = (int)fs["setting"];
	//
	fs.release();
	m_isLoaded = true;
	return true;
}

void DatasetConfig::printStats()
{
	if (!m_isLoaded)
	{
		cout << "No dataset loaded!\n";
		return;
	}
	cout << "Dataset " << m_datasetName << " starts from frame #" << m_sIdx << " to frame #" << m_eIdx;
	cout << " with offset " << m_offset << endl;
	//
	cout << "Frame size: " << m_fWidth << " * " << m_fHeight << endl;
	//
	cout << "** Frame-based ground truth:\n\t" << m_fbGTUrl << endl;
	cout << "** Trajectory-based ground truth:\n\t" << m_tbGTUrl << endl;
	//
	cout << "** Total " << m_detResUrls.size() << " detection results available:\n";
	for (size_t i = 0; i < m_detResUrls.size(); i++)
	{
		cout << "\t" << m_detResUrls[i] << endl;
	}
}