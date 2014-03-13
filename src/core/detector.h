#ifndef _OLMOT_DETECTOR_H
#define _OLMOT_DETECTOR_H

#include "defs.h"
#include "../dataset/xmlfilewrapper.h"

class Detector
{
public:
	Detector() {};
	virtual MOTDetections getDetections(const cv::Mat &img, int fIdx) = 0;
	virtual MOTDetections getDetections(int fIdx) = 0;
	virtual void getDetectionMap(const cv::Mat &img, cv::Mat &map) {};	// not implemented, but not required
	virtual ~Detector() {}
};

class PreLoadDetections: public Detector
{
public:

	PreLoadDetections(): m_loaded(false) {};

	PreLoadDetections(std::string fileURL): m_loaded(false)
	{
		setDetectionResult(fileURL);
	}

	int setDetectionResult(std::string fileURL)
	{
		m_loaded = true;
		m_fileURL = fileURL;
		m_dr = new DetectionResult(fileURL);
		return XMLFileWrapper::load(*m_dr);
	}

	virtual MOTDetections getDetections(const cv::Mat &img, int fIdx)
	{
		return getDetections(fIdx);
	}

	virtual MOTDetections getDetections(int fIdx)
	{
		if (m_loaded && fIdx < m_dr->m_nFrames)
		{
			return m_dr->m_result[fIdx];
		}
		else
		{
			MOTDetections dummy;
			return dummy;
		}
	}

private:
	bool m_loaded;
	std::string m_fileURL;
	cv::Ptr<DetectionResult> m_dr;
};

#endif // detector.h