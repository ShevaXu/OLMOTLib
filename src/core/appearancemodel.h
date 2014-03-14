#ifndef _OLMOT_APPEARANCE_MODEL_H
#define _OLMOT_APPEARANCE_MODEL_H

#include "basicrepr.h"

class AppearanceModel
{
public:
	AppearanceModel() {}

	virtual double score(cv::Ptr<Representation> templ, double factor, int mode) = 0;
	virtual double score(cv::Ptr<Representation> templ) = 0;

	virtual int update(cv::Ptr<Representation> templ, const cv::Mat &img, int mode, double factor) = 0;
	virtual int update(cv::Ptr<Representation> templ) = 0;
};

// keep the most recent template as reference
class SingleTemplateModel: public AppearanceModel
{
public:
	SingleTemplateModel(cv::Ptr<Representation> templ): m_templ(templ) {};

	double score(cv::Ptr<Representation> templ)
	{
		return m_templ->compare(templ);
	}

	double score(cv::Ptr<Representation> templ, double factor, int mode)
	{
		return this->score(templ);
	}

	int update(cv::Ptr<Representation> templ)
	{
		if (!templ.empty())
		{
			m_templ = templ;
			return 1;
		}
		else return 0;
	}

	int update(cv::Ptr<Representation> templ, const cv::Mat &img, int mode, double factor)
	{
		return this->update(templ);
	}

private:
	cv::Ptr<Representation> m_templ;
};

// keep both the original and the most recent templates as reference
class TwoTemplatesModel: public AppearanceModel
{
public:
	TwoTemplatesModel(cv::Ptr<Representation> templ, double factor = 0.5): 
	  m_original(templ), m_recent(templ), m_factor(factor) {};

	double score(cv::Ptr<Representation> templ)
	{
		return this->score(templ, m_factor);
	}

	double score(cv::Ptr<Representation> templ, double factor, int mode = 0)
	{
		if (factor > 1.0 || factor < 0.0)
		{
			factor = 0.5;
		}
		return m_original->compare(templ) * (1.0 - factor) + m_recent->compare(templ) * factor;
	}

	int update(cv::Ptr<Representation> templ)
	{
		if (!templ.empty())
		{
			m_recent = templ;
			return 1;
		}
		else return 0;
	}

	int update(cv::Ptr<Representation> templ, const cv::Mat &img, int mode, double factor)
	{
		return this->update(templ);
	}

private:
	cv::Ptr<Representation> m_original, m_recent;
	double m_factor;
};

#endif // appearancemodel.h