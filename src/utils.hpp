#ifndef _OLMOT_UTILS_HPP
#define _OLMOT_UTILS_HPP

#include <opencv2/core/core.hpp>

#define PI 3.1415169

//////////////////////////////////////////////////////////////////////////
// some helper functions:
// static or inline
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// math related
inline double getNegExp(double val, double sigma)
{
	return std::exp(-1.0 * std::pow(val / sigma, 2));
}

inline double getZeroMeanGaussianP(double val, double sigma)
{
	// zero mean gaussian
	return std::exp(std::pow(val / sigma, 2) / (-2.0)) / (sigma * std::sqrt(2.0 * PI));
}

//////////////////////////////////////////////////////////////////////////
// cv::Rect related
inline cv::Rect checkRect(const cv::Rect &rect, cv::Size imgSize)
{	
	cv::Rect imgR(cv::Point(0, 0), imgSize);
	return (imgR & rect);
}

inline cv::Rect getRectFromCenterSize(cv::Point &c, cv::Size &s)
{
	return cv::Rect(cv::Point(c.x - s.width / 2, c.y - s.height / 2), s);
}

inline cv::Rect getRectFromRootHeight(cv::Point &r, int h)
{
	return cv::Rect(r.x - h / 4, r.y - h, h / 2, h);
}

inline cv::Rect getRectFromRootSize(cv::Point &r, cv::Size &s)
{
	return cv::Rect(cv::Point(r.x - s.width / 2, r.y - s.height), s);
}

inline cv::Point getCenterFromRect(const cv::Rect &r)
{
	return cv::Point(r.x + r.width / 2, r.y + r.height / 2);
}

inline cv::Point getRootFromRect(const cv::Rect &r)
{
	return cv::Point(r.x + r.width / 2, r.y + r.height);
}

inline cv::Rect enlargeRect(cv::Rect bb, float scaleX, float scaleY)
{
	cv::Point ct = getCenterFromRect(bb);
	return cv::Rect(int(ct.x - (float)bb.width / 2 * scaleX), int(ct.y - (float)bb.height / 2 * scaleY),
		int(bb.width * scaleX + .5f), int(bb.height * scaleY + .5f));
}

inline cv::Rect averageRect(const cv::Rect &r1, const cv::Rect &r2)
{
	return cv::Rect((r1.x + r2.x) / 2, (r1.y + r2.y) / 2, (r1.width + r2.width) / 2, (r1.height + r2.height) / 2);
}

//////////////////////////////////////////////////////////////////////////
// other
template<typename _TP> 
inline void vectorAdd(std::vector<_TP> &vec, const std::vector<_TP> &vecToAdd)
{
	int i, n = vecToAdd.size();
	for (i = 0; i < n; i++)
	{
		vec.push_back(vecToAdd[i]);
	}
}

inline bool similarRect(const cv::Rect &r1, const cv::Rect &r2, double wF, double hF)
{
	int w1 = r1.width,
		h1 = r1.height,
		w2 = r2.width,
		h2 = r2.height;
	return (w1 > w2 * wF && w2 > w1 * wF &&
		h1 > h2 * hF && h2 > h1 * hF);
}

#endif	// utils.hpp