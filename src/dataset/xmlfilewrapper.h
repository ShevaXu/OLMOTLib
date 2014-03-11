#ifndef _OLMOT_XML_FILE_WRAPPER_H
#define _OLMOT_XML_FILE_WRAPPER_H

#include "../core/defs.h"

// provide static methods
class XMLFileWrapper
{
public:
	XMLFileWrapper() {};

	// load track-based ground truth
	static int load(std::string url, TrackBasedGT &gt);
	static int load(TrackBasedGT &gt)
	{
		return load(gt.m_url, gt);
	}
	// load track-based ground truth
	static int load(std::string url, FrameBasedGT &gt);
	static int load(FrameBasedGT &gt)
	{
		return load(gt.m_url, gt);
	}
	// load detection
	static int load(std::string url, DetectionResult &dr);
	static int load(DetectionResult &dr)
	{
		return load(dr.m_url, dr);
	}

	// write out tracking result
	static int writeOut(const TrackBasedResult &result, std::string outFile, int sIdx, int eIdx, int offset = 1);
	static int writeOut(const FrameBasedResult &result, std::string outFile, int sIdx, int eIdx, int offset = 1);
	// write out detection
	static int writeOut(const DetectionResult &dr, std::string outFile, int mode, int sIdx, int eIdx, int offset = 1);	

	// converter
	static int convert(TrackBasedResult &tb, FrameBasedResult &fb);

protected:
	static bool fileCheck(std::string fileName);	
};

#endif // xmlfilewrapper.h