#ifndef _OLMOT_STREAM_READER_H
#define _OLMOT_STREAM_READER_H

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class StreamReader
{
protected:
	bool m_allowPlayBack;
	int m_fCount;
	int m_fps;	// default 20 (mainly for image sequence)

	cv::Size m_fSize;
	int m_frames;

public:
	StreamReader(bool allowPlayBack): m_allowPlayBack(allowPlayBack), m_fCount(0), m_fps(20) {}

	bool allowPlayBack() { return m_allowPlayBack; }
	cv::Size getSize() { return m_fSize; }
	int getFPS() { return m_fps; }
	int getFrames() { return m_frames; }

	// get frame methods
	virtual cv::Mat getNext() = 0;
	virtual cv::Mat getPrev() = 0;

	virtual void printStats() = 0;

};

class VideoReader: public StreamReader
{
public:
	VideoReader(std::string dir);

	virtual cv::Mat getNext();
	virtual cv::Mat getPrev();

	virtual void printStats();
	
private:
	void init();

	std::string m_dir;

	cv::VideoCapture m_cap;

};

struct ImgSeqConfig
{
	std::string dir;
	std::string fileName;
	int sIdx;
	int eIdx;
	int offset;
	int frames;
};

class ImgSeqReader: public StreamReader
{
public:
	ImgSeqReader(const ImgSeqConfig &config);

	virtual cv::Mat getFrameByIdx(int idx);
	virtual cv::Mat getNext();
	virtual cv::Mat getPrev();

	virtual void printStats();

private:
	void init();

	ImgSeqConfig m_config;
	int m_currentIdx;
	std::string m_prefix, m_suffix;
	int m_numDigits;
};

#endif	// streamreader.h