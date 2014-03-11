#include "player.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

int CVPlayer::autoPlay()
{
	namedWindow("CVPlayer");
	int nFrames = m_reader->getFrames();
	createTrackbar("Frame#", "CVPlayer", &m_trackbarPos, nFrames);

	//
	int time2wait = 1000 / m_fps;

	// play loop
	bool cont = m_autoBegin;
	bool forward = true; // false for backward
	int key;
	Mat img;
	while(true)
	{
		if (forward)
			img = m_reader->getNext();
		else
			img = m_reader->getPrev();
		if (img.empty())
		{
			cout << "Run out of frames!\n";
			waitKey();
			break;
		}
		// get frame to show
		img = getFrameToShow(img, m_fCount);
		//
		imshow("CVPlayer", img);
		setTrackbarPos("Frame#", "CVPlayer", m_fCount);
		//
		if (m_recordable)
		{
			m_recorder << img;
		}
		if (cont)
		{
			key = cv::waitKey(time2wait);
			if (key == 27) // Esc: exit
			{
				m_status = PLAYER_STAT_BREAK;
				return 0;
			}
			if (key == 13) // Enter: pause
				cont = false;
		}
		while (!cont)
		{
			key = cv::waitKey(0);
			if (key == 27) // Esc
			{
				m_status = PLAYER_STAT_BREAK;
				return 0;
			}
			if (key == 13) // Enter
			{
				cont = true;
				forward = true;
				break;
			}
			if (key == 32) // Space: step forward
			{
				forward = true;
				break;
			}
			if (key == 'b') // b: step backward
			{
				if (m_allowPlayBack)
				{
					forward = false;
					m_fCount -= 2;
					break;
				}
				else
					cout << "Play back is not allowed!\n";				
			}
			//
			if (key == 's' || key == 'S') // save image
			{
				stringstream buffer;
				buffer << "img" << m_fCount << ".jpg";
				string strImg = buffer.str();
				imwrite(strImg, img);
				cout << "---Save image " << strImg << "!---\n";
			}
		}	
		//
		m_fCount++;
	}
	m_status = PLAYER_STAT_END;
	return 1;
}

bool CVPlayer::setRecordFile( std::string fileName, int frameRate )
{
	// can try CV_FOURCC('M','P','4','2') or CV_FOURCC('D','I','V','X')
	// now use CV_FOURCC('D','I','V','3') with .avi file
	m_recorder.open(fileName.c_str(), CV_FOURCC('D','I','V','3'), frameRate, m_reader->getSize());
	m_recordable = m_recorder.isOpened();
	if (!m_recordable)
	{
		cout << "Recording error in initializing!\n";
	}
	return m_recordable;
}