#include "detectorwrapper.h"

#include <iostream>

using namespace std;
using namespace cv;

HOGPeopleDetector::HOGPeopleDetector()
{
	m_hog = new HOGDescriptor();
	m_hog->setSVMDetector(HOGDescriptor:: getDefaultPeopleDetector());
}

MOTDetections HOGPeopleDetector::getDetections( const cv::Mat &img, int fIdx )
{
	std::vector<Rect> locs;

	// TODO: param setting
	m_hog->detectMultiScale(img, locs);	// all default
	//m_hog.detectMultiScale(img, locs, 0, Size(8,8), Size(32,32), 1.1, 2);

	// conversion
	MOTDetections dets;
	for(size_t i = 0; i < locs.size(); i++ )
	{
		Rect r = locs[i];
		// the HOG detector returns slightly larger rectangles than the real objects.
		// so we slightly shrink the rectangles to get a nicer output.
		r.x += cvRound(r.width * 0.1);
		r.width = cvRound(r.width * 0.8);
		r.y += cvRound(r.height * 0.07);
		r.height = cvRound(r.height * 0.8);

		dets.push_back(Detection(r, 1.0));
	}

	return dets;
}

//////////////////////////////////////////////////////////////////////////

LatentSVMWrapper::LatentSVMWrapper( const std::vector<string>& filenames, float confThresh /*= 0.5*/ ): m_confThresh(confThresh)
{
	m_detector = new LatentSvmDetector();	// do not provide file names yet

	if (!m_detector->load(filenames))	// load at least one model to be true
	{
		cout << "Fail to load any model!\n";
	}
	else
	{
		int nC = m_detector->getClassCount();	// should > 0
		if (nC > 0)
		{
			vector<string> classes = m_detector->getClassNames();
			for (int i = 0; i < nC; i++)
			{
				cout << i << ": " << classes[i] << endl;
			}
		}		
	}
}

MOTDetections LatentSVMWrapper::getDetections( const cv::Mat &img, int fIdx )
{
	vector<LatentSvmDetector::ObjectDetection> rawDets;
	m_detector->detect(img, rawDets);

	MOTDetections dets;

	// filter
	for(size_t i = 0; i < rawDets.size(); i++ )
	{
		if (rawDets[i].score < m_confThresh)
		{
			break;
		}

		// conversion
		dets.push_back(Detection(rawDets[i].rect, rawDets[i].score));
	}

	return dets;
}

//////////////////////////////////////////////////////////////////////////

CascadeDetector::CascadeDetector(const string& filename)
{
	m_cascade = new CascadeClassifier(filename);
}

MOTDetections CascadeDetector::getDetections( const cv::Mat &img, int fIdx )
{
	std::vector<Rect> locs;

	// TODO: param setting
	m_cascade->detectMultiScale(img, locs);	// all default

	// conversion
	MOTDetections dets;
	for(size_t i = 0; i < locs.size(); i++ )
	{
		dets.push_back(Detection(locs[i], 1.0));
	}

	return dets;
}
