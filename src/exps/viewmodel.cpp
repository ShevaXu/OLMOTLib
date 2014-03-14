#include "viewmodel.h"

using namespace cv;
using namespace std;

float ViewModel::computeYcGivenV0(const std::vector<DetectionDataPoint> &data)
{
	float temp = 0;
	size_t n = data.size();
	float v0 = m_horizon * m_sceneSize.height;
	for (size_t i = 0; i < n; i++)
	{
		temp += data[i].h / (data[i].v - v0);
	}
	temp /= n;
	return m_objHeight / temp;
}

float ViewModel::computeV0GivenYc( const std::vector<DetectionDataPoint> &data )
{
	float temp = 0;
	size_t n = data.size();
	for (size_t i = 0; i < n; i++)
	{
		temp += data[i].v - m_camHeight * data[i].h / m_objHeight;
	}
	temp /= n;
	return temp / m_sceneSize.height;
}

void ViewModel::estimate( const cv::Mat &gray, const MOTDetections &rawDets )
{
	std::vector<DetectionDataPoint> detData;
	for (size_t i = 0; i < rawDets.size(); i++ )
	{
		Rect bb = rawDets[i].bb;
		detData.push_back(DetectionDataPoint((float)bb.br().y, (float)bb.height));
	}

	// assume detector fails
	m_isVPsValid = false;
	if (m_estCount % m_vpsFreq == 0)	// every m_freq frame
	{
		// use detector
		m_VPDetector->processImage(gray, 3);
		m_vpsHorizon = m_VPDetector->getHorizon();
		m_vp = m_VPDetector->getVanishPoint();
		
		if (-1.0f == m_horizon)	// first frame
		{
			m_isVPsValid = true;	// have to be valid
			if(-1.0f == m_vpsHorizon) 
				m_vpsHorizon = 0.5f;	// if fails				
		}
		else
		{
			if (std::abs(m_vpsHorizon - m_horizon) < m_vpsValidThresh)	// no abrupt change
			{
				m_isVPsValid = true;				
			}
			else
			{
				if (m_verbose)
				{
					cout << "Dump vps estimation: " << m_vpsHorizon << endl;
				}
			}
		}
	}	

	estimateIter(detData);
	m_estCount++;

	// update camera parameters
	float theta = 2 * atan((m_camCY-m_horizon*m_sceneSize.height) / 2.0f / m_focalY); // in pixel
	m_sinT = sin(theta);
	m_cosT = cos(theta);

	return;
}

void ViewModel::estimateIter(const std::vector<DetectionDataPoint> &detData)
{
	if (-1.0f == m_horizon) // first frame
	{
		
		if (-1.0f == m_knownCamH)	// unknown camera height
		{
			// compute v0
			m_horizon = (m_priorMiu * m_vpsSigma + m_vpsHorizon * m_priorSigma) / (m_priorSigma + m_vpsSigma);
			// compute yc
			m_camHeight = computeYcGivenV0(detData);
		}
		else
		{
			m_camHeight = m_knownCamH;
			m_estHorizon = computeV0GivenYc(detData);
			float t1 = m_vpsSigma * m_estSigma,
				t2 = m_priorSigma * m_estSigma,
				t3 = m_priorSigma * m_vpsSigma;
			m_horizon = (m_priorMiu * t1 + m_vpsHorizon * t2 + m_estHorizon * t3) / (t1+t2+t3);
		}
	}
	else
	{
		if (m_estCount < m_nInit)	// first phase: initialization
		{
			// get data estimation
			m_estHorizon = computeV0GivenYc(detData);
			//
			if (m_isVPsValid)
			{
				float t1 = m_vpsSigma * m_estSigma * m_propaSigma,
					t2 = m_priorSigma * m_estSigma * m_propaSigma,
					t3 = m_priorSigma * m_vpsSigma * m_propaSigma,
					t4 = m_priorSigma * m_vpsSigma * m_estSigma;
				m_horizon = (m_priorMiu * t1 + m_vpsHorizon * t2 + m_estHorizon * t3 + m_horizon * t4) / (t1+t2+t3+t4);
			}
			else
			{
				float t1 = m_estSigma * m_propaSigma,
					t3 = m_priorSigma * m_propaSigma,
					t4 = m_priorSigma * m_estSigma;
				m_horizon = (m_priorMiu * t1 + m_estHorizon * t3 + m_horizon * t4) / (t1+t3+t4);			
			}
			
			if (-1.0f == m_knownCamH)	// unknown camera height
			{
				// recompute yc
				m_camHeight = (1.0f - m_camHPrior) * computeYcGivenV0(detData) + m_camHPrior * m_camHeight;				
			}
			else
				m_camHeight = m_knownCamH;			
		}
		else // second phase: stable camera height, no prior
		{
			// get data estimation
			m_estHorizon = computeV0GivenYc(detData);
			//
			if (m_isVPsValid)
			{
				float t2 = m_estSigma * m_propaSigma,
					t3 = m_vpsSigma * m_propaSigma,
					t4 = m_vpsSigma * m_estSigma;
				m_horizon = (m_vpsHorizon * t2 + m_estHorizon * t3 + m_horizon * t4) / (t2+t3+t4);
			}
			else
			{
				m_horizon = (m_estHorizon * m_propaSigma + m_horizon * m_estSigma) / (m_estSigma + m_propaSigma);
			}			
		}
	}

	if (m_verbose)
	{
		cout << "VPs: " << m_vpsHorizon << endl;
		cout << "est: " << m_estHorizon << endl;
		cout << "Horizon: " << m_horizon << "-" << m_horizon * m_sceneSize.height << endl;
		cout << "Camera height: " << m_camHeight << endl;
	}
}

int ViewModel::filter( const MOTDetections &rawDets, std::vector<float> &indicator )
{
	int filtered = 0;	// counter
	std::vector<DetectionDataPoint> data;

	size_t n = rawDets.size();
	for (size_t i = 0; i < n; i++ )
	{
		Rect bb = rawDets[i].bb;
		data.push_back(DetectionDataPoint((float)bb.br().y, (float)bb.height));
		indicator.push_back(0.0f);
	}	

	float v0 = m_horizon * m_sceneSize.height,
		yc = m_camHeight,
		y = m_objHeight;
	
	if (indicator.size() != n)
	{
		indicator.resize(n);
	}
	for (size_t i = 0; i < n; i++)
	{
		if (data[i].v <= v0)
		{
			indicator[i] = -1.0f;	// above horizon
			filtered++;
			continue;
		}
		float ratio = yc / y * data[i].h / (data[i].v - v0);
		if (ratio < 0.5 || ratio > 1.4)	// wrong ratio
		{
			indicator[i] = -1.0f;
			filtered++;
		}
		else
			indicator[i] = std::exp(-abs(ratio - 1.0f));
	}
	//
	return filtered;
}

Point3f ViewModel::getProjection( Point posImg )
{
	Point3f posWorld;

	// in pixel
	float u = (float)posImg.x, 
		v = (float)posImg.y;
	// z = fy*yc / (fy*st-(vc-vb)*ct)
	float z = m_focalY * m_camHeight / (m_focalY * m_sinT - (m_camCY - v) * m_cosT);	
	// (u*z - uc*z*ct) / fx

	posWorld.x = (u - m_camCX * m_cosT) * z / m_focalX;
	posWorld.y = 0;	// always zero
	posWorld.z = z;

	return posWorld;
}

int ViewModel::getStdHeight( int v )
{
	// y = yc * hi / (v - v0)
	// so, hi = y * (v - v0) / yc
	return (int)(m_objHeight * (v - m_horizon * m_sceneSize.height) / m_camHeight);
}

cv::Point ViewModel::getBackProjection( cv::Point3f pos3D )
{
	float x = pos3D.x,
		z = pos3D.z;
	Point ret;
	// u = (f*x + uc*z*ct) / z
	ret.x = (int)((m_focalX * x + m_camCX * z * m_cosT) / z + 0.5f);	// rounding
	// v = vc - f/ct * (st - yc/z)
	ret.y = (int)(m_camCY - m_focalY / m_cosT * (m_sinT - m_camHeight / z) + 0.5f);	// rounding

	return ret;
}