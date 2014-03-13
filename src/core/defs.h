#ifndef _OLMOT_DEFS_H
#define _OLMOT_DEFS_H

#include <opencv2/core/core.hpp>

//////////////////////////////////////////////////////////////////////////
// alias

typedef cv::Rect BoundingBox;

//////////////////////////////////////////////////////////////////////////
// simple combinator

struct IndexSet
{
	int sIdx, eIdx, offset;
};

// for 3D
struct MOTGeoInfo
{
	float m_horizon;
	cv::Point m_vp;
	float m_camHeight;
};

//////////////////////////////////////////////////////////////////////////
// more complex combinator
// no functions
// require empty constructor, if has other constructors
// no "m_" prefix for attributes, all public

// basic info, which might changes every frame
struct ObjBasicInfo 
{
	ObjBasicInfo() {};
	ObjBasicInfo(const BoundingBox &_bb): bb(_bb) {};
	ObjBasicInfo(const BoundingBox &_bb, const cv::Point3f &_pos3D, int _state): bb(_bb), pos3D(_pos3D), state(_state) {};
	
	// attributes	
	BoundingBox bb;
	cv::Point3f pos3D;
	int state;	// occlusion state
};

// for frame-wise output
struct ObjFrameInfo	
{
	ObjFrameInfo(): id(-1) {};
	ObjFrameInfo(int _id, const ObjBasicInfo &_info, int _category = 0): id(_id), category(_category), info(_info) {};
	
	// attributes
	int id;
	int category;
	ObjBasicInfo info;	
};

// for track-wise output
struct ObjTrackInfo
{
	ObjTrackInfo() {};
	ObjTrackInfo(int _fIdx, ObjBasicInfo &_info): fIdx(_fIdx), info(_info) {};

	// attributes
	int fIdx;
	ObjBasicInfo info;
};

// stacked in spatial dimension
typedef std::vector<ObjFrameInfo> MOTOutput;

// result for a single target: temporal vector
struct MOTTrajectory
{
	MOTTrajectory() {};
	MOTTrajectory(int _id, int _category): id(_id), category(_category) {};

	// attributes
	int id;
	int category;
	IndexSet idx;
	std::vector<ObjTrackInfo> trac;
};

struct Detection
{
	Detection() {};
	Detection(const BoundingBox &_bb, double _confidence = 0.0): bb(_bb), confidence(_confidence) {};

	// attributes
	BoundingBox bb;
	double confidence;
};

typedef std::vector<Detection> MOTDetections;

struct DetEvalResult
{
	DetEvalResult(): falseAlarms(0), missDets(0), goodDets(0) {}

	int falseAlarms,
		missDets,
		goodDets;
};

//////////////////////////////////////////////////////////////////////////
// class with functions
// "m_" prefix for attributes

class FrameBasedResult
{
public:
	FrameBasedResult(): m_with3D(false) {};
	FrameBasedResult(std::string fileURL, bool with3D = false): m_with3D(with3D), m_url(fileURL) {};
	
	void add(MOTOutput &output)
	{
		m_result.push_back(output);
	}

	// attributes
	bool m_with3D;
	std::string m_url;
	int m_nFrames,
		m_nTracks;
	IndexSet m_idx;
	std::vector<MOTOutput> m_result;
};

class TrackBasedResult
{
public:
	TrackBasedResult(): m_with3D(false) {};
	TrackBasedResult(std::string fileURL, bool with3D = false): m_with3D(with3D), m_url(fileURL) {};

	void add(MOTTrajectory &trac)
	{
		m_result.push_back(trac);
	}

	// attributes
	bool m_with3D;
	std::string m_url;
	int m_nFrames,
		m_nTracks;
	IndexSet m_idx;
	std::vector<MOTTrajectory> m_result;
};

// result and ground truth are the same
typedef TrackBasedResult TrackBasedGT;
typedef FrameBasedResult FrameBasedGT;

class DetectionResult
{
public:
	DetectionResult() {};
	DetectionResult(std::string fileURL): m_url(fileURL) {};

	void add(MOTDetections &det)
	{
		m_result.push_back(det);
	}

	// attributes
	std::string m_url;
	int m_nFrames;
	IndexSet m_idx;	
	std::vector<int> m_fIdxes;	// no specific struct for id, detections pair
	std::vector<MOTDetections> m_result;
};

//////////////////////////////////////////////////////////////////////////
// basic representation
class Representation 
{
public:
	Representation() {};
	// key
	virtual double compare(cv::Ptr<Representation> other, int mode) = 0;
	// for default behavior
	virtual double compare(cv::Ptr<Representation> other) = 0;
	// for verbose
	virtual cv::Mat toMat() = 0;
	virtual std::string toString() = 0;

	virtual ~Representation() {};
};

typedef std::vector<cv::Ptr<Representation>> MOTTemplates;

//////////////////////////////////////////////////////////////////////////

struct DetectionWrapper
{
	DetectionWrapper() {};
	DetectionWrapper(BoundingBox _bb, double _confidence): bb(_bb), confidence(_confidence), associated(false), repr(NULL) {};
	
	DetectionWrapper(const Detection &det): bb(det.bb), confidence(det.confidence), associated(false), repr(NULL) {};

	// detection info
	BoundingBox bb;
	double confidence;
	bool associated;
	// the representation
	cv::Ptr<Representation> repr;
	// if has 3D info
	cv::Point3f pos3D;	
	// for debug
	int idx;
};

typedef cv::Ptr<DetectionWrapper> DetectionWrapperPtr;
typedef std::vector<DetectionWrapperPtr> FinalDetections;

#endif // defs.h