#include "streamreader.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace cv;

//////////////////////////////////////////////////////////////////////////

VideoReader::VideoReader( string dir ) : m_dir(dir), StreamReader(false)
{
	m_cap.open(dir);
	if (m_cap.isOpened())
	{
		init();
	}
	else
		cout << "Can not open video " << dir << endl;
}

void VideoReader::init()
{
	m_frames = (int)m_cap.get(CV_CAP_PROP_FRAME_COUNT);
	if (m_frames < 0 || m_frames > 99999) // error happen
	{
		m_frames = 10000;
	}
	int h = (int)m_cap.get(CV_CAP_PROP_FRAME_HEIGHT),
		w = (int)m_cap.get(CV_CAP_PROP_FRAME_WIDTH);
	m_fSize = Size(w, h);
	m_fps = (int)m_cap.get(CV_CAP_PROP_FPS);
}

Mat VideoReader::getNext()
{
	Mat frame;
	m_cap >> frame;
	if (frame.empty())
		cout << "Video run out of frames!\n";
	return frame;
}

Mat VideoReader::getPrev()
{
	return getNext();
}

void VideoReader::printStats()
{
	cout << "Video: " << m_dir << endl;
	cout << "Info: " << m_fSize << " with " << m_frames << " frames at " << m_fps << " fps\n";
}

//////////////////////////////////////////////////////////////////////////

ImgSeqReader::ImgSeqReader( const ImgSeqConfig &config ): m_config(config), StreamReader(true)
{
	init();
}

void ImgSeqReader::init()
{
	m_currentIdx = m_config.sIdx;
	m_frames = m_config.frames;

	// get prefix and suffix and num of #
	m_prefix = m_config.fileName.substr(0, m_config.fileName.find_first_of("#"));
	m_suffix = m_config.fileName.substr(m_config.fileName.find_last_of("#")+1);
	m_numDigits = (int)(m_config.fileName.find_last_of("#") + 1 - m_config.fileName.find_first_of("#"));

	// get size
	Mat temp = getFrameByIdx(m_config.sIdx);
	m_fSize = temp.size();
}

Mat ImgSeqReader::getFrameByIdx( int index )
{
	if (index >= m_config.sIdx && index <= m_config.eIdx)
	{
		stringstream buffer;
		buffer << m_config.dir << "\\" << m_prefix << setw(m_numDigits) << setfill('0') << index << m_suffix;
		string url = buffer.str();

		Mat frame = imread(url);
		if(frame.empty())
		{
			cout << "Can not load image " << url << endl;
			return Mat();
		}
		return frame;
	}
	else if (index == m_config.eIdx + m_config.offset)
	{
		return Mat();
	}
	else
	{
		cout << "Error @ImgSeqReader::getFrameByIdx: Wrong index - " << index << "!\n";
		return Mat();
	}
}

Mat ImgSeqReader::getNext()
{
	Mat frame = getFrameByIdx(m_currentIdx);
	if (!frame.empty())
		m_currentIdx += m_config.offset;
	return frame;
}

Mat ImgSeqReader::getPrev()
{
	// maybe not be a proper call
	if (!m_allowPlayBack) return getNext();

	Mat frame;
	if (m_currentIdx < m_config.sIdx + 2 * m_config.offset) // back to start
	{
		frame = getFrameByIdx(m_config.sIdx);
	}
	else
	{
		frame = getFrameByIdx(m_currentIdx - m_config.offset * 2);
		if (!frame.empty())
			m_currentIdx -= m_config.offset;
	}
	return frame;
}

void ImgSeqReader::printStats()
{
	cout << "Image sequence at " << m_config.dir << endl;
	cout << "Config Format: " << m_config.fileName << " from " << m_config.sIdx << " to " << m_config.eIdx << " with " << m_config.offset << endl;
	cout << "Info: " << m_fSize << " with " << m_frames << " frames at " << m_fps << " fps\n";
}