#ifndef _OLMOT_VIEW_MODEL_H
#define _OLMOT_VIEW_MODEL_H

#include "../core/defs.h"

#include "../wrapper/VanishPointDetector.h"

struct DetectionDataPoint
{
	DetectionDataPoint(float bottom, float height) : v(bottom), h(height) {}
	float v, h;
};

struct ViewModelInfo
{
	int horizon;
	float camHeight;
};

// setSigmas(float prior, float estimation, float vps, float propa)
// setSigmas(0.1f, 0.5f, 0.1f, 0.1f);	// second best
// setSigmas(0.5f, 0.5f, 0.1f, 0.5f);
// setSigmas(0.5f, 0.5f, 0.1f, 0.1f);	// current
struct ViewModelParams
{
	ViewModelParams(): vpsFreq(99999), vpsValidThresh(0.05f), vpsSigma(0.1f), estSigma(0.5f), priorSigma(0.5f), propaSigma(0.1f),
		priorMiu(0.5f), knownCamHeight(-1.0f), objHeight(1.7f), nInit(10), priorCamH(0.9f),
		focalX(500.0f), focalY(500.0f), camCX(0.5f), camCY(0.5f)
	{};

	// frequency of perform vanishings point detection
	int vpsFreq;	
	float vpsValidThresh;
	// sigmas for each evidence
	float vpsSigma, estSigma, priorSigma, propaSigma;
	// prior (0.0~1.0)
	float priorMiu;
	// cam height, if unknown, set to -1.0
	float knownCamHeight;	// (0.0~1.0)
	// known object height
	float objHeight;	// in meter
	// # initialization-frames
	int nInit;
	// cam height update factor
	float priorCamH;
	// camera focal
	float focalX,
		focalY;
	// optical center, if < 1.0, must multiply with the image heigh and width
	float camCX,
		camCY;
};

class ViewModel
{
public:
	ViewModel(cv::Size sceneSize, ViewModelParams params = ViewModelParams(), bool verbose = false):
		m_sceneSize(sceneSize), m_verbose(verbose)
	{
		m_VPDetector = new VanishPointDetector(sceneSize);
		//
		setParams(params);
		// data init
		m_horizon = -1.0f;	
		m_camHeight = -1.0f;
		m_estCount = 0;
	}

	// in process, if cam height is got by other means, set it through this
	void setCamHeight(float yc) { m_knownCamH = yc; }	

	// main interfaces
	void estimate(const cv::Mat &gray, const MOTDetections &rawDets);
	int filter(const MOTDetections &rawDets, std::vector<float> &indicator);

	//////////////////////////////////////////////////////////////////////////
	// getter
	ViewModelInfo getGeo()
	{
		ViewModelInfo info;
		info.horizon = (int)(m_horizon * m_sceneSize.height);
		info.camHeight = m_camHeight;
		return info;
	}

	// for mapping
	cv::Point3f getProjection(cv::Point posImg);
	cv::Point getBackProjection(cv::Point3f pos3D);
	int getStdHeight(int v);	

	~ViewModel()
	{
		delete m_VPDetector;
	}

protected:
	float computeYcGivenV0(const std::vector<DetectionDataPoint> &data);
	float computeV0GivenYc(const std::vector<DetectionDataPoint> &data);

	void estimateIter(const std::vector<DetectionDataPoint> &detData);

	//void setParams()
	//{
	//	// horizon params
	//	m_priorMiu = 0.5f;		
	//	//setSigmas(0.1f, 0.5f, 0.1f, 0.1f); // second best
	//	//setSigmas(0.5f, 0.5f, 0.1f, 0.5f);
	//	setSigmas(0.5f, 0.5f, 0.1f, 0.1f);
	//	m_nInit = 10;
	//	// human height
	//	m_yHeight = 1.7f;	// in meter, for pedestrian
	//	// cam params
	//	m_focalX = 500.0f;	// in pixel
	//	m_focalY = 500.0f;
	//	m_camCX = 0.5f * m_sceneSize.width;	// middle, in pixel
	//	m_camCY = 0.5f * m_sceneSize.height;
	//	// yc prior, for v1
	//	m_ycPrior = 0.9f;
	//}

	void setParams(const ViewModelParams &params)
	{
		m_vpsFreq = params.vpsFreq;		
		m_vpsValidThresh = params.vpsValidThresh;
		//
		m_priorSigma = params.priorSigma;
		m_estSigma = params.estSigma;
		m_vpsSigma = params.vpsSigma;
		m_propaSigma = params.propaSigma;
		// 
		m_priorMiu = params.priorMiu;
		//
		m_knownCamH = params.knownCamHeight;
		// 
		m_objHeight = params.objHeight;
		//
		m_nInit = params.nInit;
		// 
		m_camHPrior = params.priorCamH;
		// cam params
		m_focalX = params.focalX;	
		m_focalY = params.focalY;
		//
		if (params.camCX < 1.0f)		
			m_camCX = params.camCX * m_sceneSize.width;	// middle, in pixel		
		else
			m_camCX = params.camCX;
		if (params.camCY < 1.0f)		
			m_camCY = params.camCY * m_sceneSize.width;	// middle, in pixel		
		else
			m_camCY = params.camCY;		
	}

private:	
	cv::Size m_sceneSize;
	bool m_verbose;

	// vanishing points evidence
	VanishPointDetector *m_VPDetector;
	int m_vpsFreq;
	float m_vpsValidThresh;
	bool m_isVPsValid;
	float m_vpsHorizon,
		m_vpsSigma;
	// detection data estimation
	float m_estHorizon,
		m_estSigma;
	// prior	
	float m_priorMiu,
		m_priorSigma;
	// final decision and propagation sigma
	float m_horizon,
		m_propaSigma;
	// cam height, human height
	float m_camHeight,
		m_knownCamH,
		m_objHeight;
	// two-phase params
	int m_estCount,
		m_nInit;
	float m_camHPrior;
	// vanish point
	cv::Point m_vp;
	// camera
	float m_focalX,
		m_focalY;
	float m_camCX,
		m_camCY;
	float m_sinT,
		m_cosT;
};

typedef cv::Ptr<ViewModel> ViewModelPtr;

#endif	// viewmodel.h