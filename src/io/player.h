#ifndef _OLMOT_PLAYER_H
#define _OLMOT_PLAYER_H

#include "streamreader.h"

class CVPlayer
{
public:

	static const int PLAYER_STAT_EMPTY = 0;
	static const int PLAYER_STAT_READY = 1;
	static const int PLAYER_STAT_BREAK = 2;
	static const int PLAYER_STAT_END = 3;

	CVPlayer(bool autoBegin): 
	  m_autoBegin(autoBegin), m_allowPlayBack(false), m_recordable(false), m_fCount(0), m_status(0) {};
	
	void setReader(cv::Ptr<StreamReader> reader)
	{
		m_reader = reader;
		m_allowPlayBack = reader->allowPlayBack();
		m_fps = m_reader->getFPS();
		m_status = PLAYER_STAT_READY;
	}

	virtual bool setRecordFile(std::string fileName, int frameRate);	

	virtual cv::Mat getFrameToShow(const cv::Mat &frame, int fIdx)
	{
		return frame;
	}

	virtual int autoPlay();

	// getter
	int getStatus() { return m_status; }

protected:
	bool m_autoBegin;
	bool m_allowPlayBack;
	bool m_recordable;	
	int m_fCount;
	int m_status;

	cv::Ptr<StreamReader> m_reader;
	int m_fps;
	int m_trackbarPos;
	cv::VideoWriter m_recorder;
};

#endif // player.h