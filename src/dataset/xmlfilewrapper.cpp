#include "xmlfilewrapper.h"

// to disable warning in contributing code
// should remove when publishing for cross-platform
#pragma warning (push, 0)
#include "../../contribs/xml/xml.h"
#pragma warning (pop)

#include <iostream>
#include <fstream>
#include <set>

using namespace cv;
using namespace std;

int XMLFileWrapper::load( std::string url, TrackBasedGT &gt )
{
	if (!fileCheck(url)) return 0;

	gt.m_url = url;
	XML *xmlf = new XML(gt.m_url.c_str());
	if (!xmlf->IntegrityTest() || xmlf->ParseStatus() != 0)
	{
		cout << "XML error!\n";
		delete xmlf;
		return 0;
	}

	XMLElement* r = xmlf->GetRootElement(); // "Video" element
	char tempC[20] = {0};
	r->GetElementName(tempC);
	String rootElem = tempC;
	if (rootElem != "Video")
	{
		cout << "Wrong format! MOT trajectory based ground truth format expected!\n";
		return 0;
	}

	// get stats
	int nC = (int)r->GetChildrenNum();
	gt.m_nFrames = nC;
	cout << "Root element has " << nC << " children.\n";
	//
	gt.m_idx.sIdx = r->FindVariableZ("start_frame")->GetValueInt();
	gt.m_idx.eIdx = r->FindVariableZ("end_frame")->GetValueInt();

	XMLElement *ch1, *ch2; // child node with different depth level
	//XMLVariable* v;
	int oH, oW, oX, oY;	// temp object info

	// iterates over trajectories	
	for(int i = 0 ; i < nC ; i++)
	{
		MOTTrajectory tempTrac;
		//
		ch1 = r->GetChildren()[i];		// level 1: "Trajectory" element
		tempTrac.id = ch1->FindVariableZ("obj_id")->GetValueInt();
		tempTrac.idx.sIdx = ch1->FindVariableZ("start_frame")->GetValueInt();
		tempTrac.idx.eIdx = ch1->FindVariableZ("end_frame")->GetValueInt();
		//v = ch1->FindVariableZ("obj_type");
		//v->GetValue(tempC);
		//tempTrac.objTypeStr = tempC;
		
		int nF = (int)ch1->GetChildrenNum();
		for (int j = 0; j < nF; j++)
		{
			ch2 = ch1->GetChildren()[j];	// level 2: "Frame" element
			//
			int idx = ch2->FindVariableZ("frame_no")->GetValueInt();
			// convert to a Rect
			oH = (int)ch2->FindVariableZ("height")->GetValueFloat();
			oW = (int)ch2->FindVariableZ("width")->GetValueFloat();
			oX = (int)ch2->FindVariableZ("x")->GetValueFloat();
			oY = (int)ch2->FindVariableZ("y")->GetValueFloat();
			Rect bb = Rect(oX, oY, oW, oH);
			tempTrac.trac.push_back(ObjTrackInfo(idx, ObjBasicInfo(bb)));
		}
		// calculate offset (approximation)
		tempTrac.idx.offset = (tempTrac.idx.eIdx - tempTrac.idx.eIdx + 1) / nF;	
		if (tempTrac.idx.offset < 1)
			tempTrac.idx.offset = 1;
		//
		gt.add(tempTrac);
	}
	delete xmlf;

	// set stat
	gt.m_nTracks = gt.m_result.size();
	// approximation, might be wrong
	gt.m_idx.offset = gt.m_result[0].idx.offset;
	//
	gt.m_nFrames = (gt.m_idx.eIdx - gt.m_idx.sIdx + 1) / gt.m_idx.offset;	

	cout << "Successfully loaded - " << gt.m_url << endl;
	return 1;
}

int XMLFileWrapper::load( std::string url, FrameBasedGT &gt )
{	
	if (!fileCheck(url)) return 0;

	gt.m_url = url;
	XML *xmlf = new XML(gt.m_url.c_str());
	if (!xmlf->IntegrityTest() || xmlf->ParseStatus() != 0)
	{
		cout << "XML error!\n";
		delete xmlf;
		return 0;
	}

	XMLElement* r = xmlf->GetRootElement(); // "dataset" element	
	char tempC[20] = {0};
	r->GetElementName(tempC);
	String rootElem = tempC;
	if (rootElem != "dataset")
	{
		cout << "Wrong format! CVML frame based ground truth format expected!\n";
		return 0;
	}

	// get stats
	int nC = (int)r->GetChildrenNum();
	gt.m_nFrames = nC;
	cout << "Root element has " << nC << " children.\n";
	//
	gt.m_idx.sIdx = r->GetChildren()[0]->FindVariableZ("number")->GetValueInt();
	gt.m_idx.eIdx = r->GetChildren()[nC - 1]->FindVariableZ("number")->GetValueInt();
	// assume the offset is fixed
	gt.m_idx.offset = r->GetChildren()[1]->FindVariableZ("number")->GetValueInt() - gt.m_idx.sIdx;

	XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
	int oId, oH, oW, oXc, oYc;	// temp object info
	// to count #objects
	std::set<int> idSet;
	
	// iterates over frames	
	for(int i = 0 ; i < nC ; i++)
	{
		MOTOutput fOutput;
		//
		ch1 = r->GetChildren()[i];		// level 1: "frame" element
		ch2 = ch1->GetChildren()[0];	// level 2: "objectList" element
		int nCO = (int)ch2->GetChildrenNum();
		for (int j = 0; j < nCO; j++)
		{
			ch3 = ch2->GetChildren()[j];	// level 3: "object" element
			oId = ch3->FindVariableZ("id")->GetValueInt();
			idSet.insert(oId);

			ch4 = ch3->GetChildren()[0];	// level 4: "box" element
			oH = (int)ch4->FindVariableZ("h")->GetValueFloat();
			oW = (int)ch4->FindVariableZ("w")->GetValueFloat();
			oXc = (int)ch4->FindVariableZ("xc")->GetValueFloat();
			oYc = (int)ch4->FindVariableZ("yc")->GetValueFloat();
			// convert h, w, xCenter, yCenter to Rect			
			int oX = oXc - oW / 2,
				oY = oYc - oH / 2;			
			/************************************************************************/
			/*
			!!!!!! Notice: x or y may be < 0 (part of the object out of frame) !!!!!!!!!                                                                     
			*/
			/************************************************************************/						
			// allow out of frame (<0), handle in player
			Rect bb = Rect(oX, oY, oW, oH);
			// 
			fOutput.push_back(ObjFrameInfo(oId, ObjBasicInfo(bb)));			
		}
		gt.add(fOutput);
	}

	delete xmlf;

	// assume id is 1-based continuously indexed
	gt.m_nTracks = idSet.size();

	cout << "Successfully loaded - " << gt.m_url << endl;
	return 1;
}

int XMLFileWrapper::load( std::string url, DetectionResult &dr )
{
	if (!fileCheck(url)) return 0;	

	dr.m_url = url;
	XML *xmlf = new XML(dr.m_url.c_str());
	if (!xmlf->IntegrityTest() || xmlf->ParseStatus() != 0)
	{
		cout << "XML error!\n";
		delete xmlf;
		return 0;
	}

	XMLElement* r = xmlf->GetRootElement(); 
	// check format
	char tempC[20] = {0};
	r->GetElementName(tempC);
	String rootElem = tempC;

	if ("Video" == rootElem)	// "Video" element format
	{
		// get stats
		int nC = (int)r->GetChildrenNum();
		dr.m_nFrames = nC;
		cout << "Root element has " << nC << " children.\n";		
		dr.m_idx.sIdx = r->FindVariableZ("start_frame")->GetValueInt();
		dr.m_idx.eIdx = r->FindVariableZ("end_frame")->GetValueInt();
		
		XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
		int oH, oW, oX, oY;	// temp object info

		// iterates over frames	
		for(int i = 0 ; i < nC ; i++)
		{
			MOTDetections dets;
			//
			ch1 = r->GetChildren()[i];		// level 1: "Frame" element
			int idx = ch1->FindVariableZ("no")->GetValueInt();
			ch2 = ch1->GetChildren()[0];	// level 2: "ObjectList" element
			int nO = ch2->GetChildrenNum();		
			for (int j = 0; j < nO; j++)
			{
				ch3 = ch2->GetChildren()[j];	// level 3: "Object" element

				double conf;
				if (ch3->FindVariable("score") == -1)
					conf = 0.0;
				else 
					conf = ch3->FindVariableZ("score")->GetValueFloat();
				ch4 = ch3->GetChildren()[0];	// level 4a: "Rect" element
				oH = (int)ch4->FindVariableZ("height")->GetValueFloat();
				oW = (int)ch4->FindVariableZ("width")->GetValueFloat();
				oX = (int)ch4->FindVariableZ("x")->GetValueFloat();
				oY = (int)ch4->FindVariableZ("y")->GetValueFloat();
				Rect bb = Rect(oX, oY, oW, oH);
				//
				dets.push_back(Detection(bb, conf));
			}		
			// sync
			dr.m_result.push_back(dets);
			dr.m_fIdxes.push_back(idx);
		}
	}
	else if ("dataset" == rootElem)	// another format: for the detection result by Andriyenko
	{
		// get stats
		int nC = (int)r->GetChildrenNum();
		dr.m_nFrames = nC;
		cout << "Root element has " << nC << " children.\n";

		// no direct access of start&end index
		dr.m_idx.sIdx = r->GetChildren()[0]->FindVariableZ("number")->GetValueInt();
		dr.m_idx.eIdx = r->GetChildren()[nC - 1]->FindVariableZ("number")->GetValueInt();

		XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
		float oH, oW, oXc, oYc;	// temp object info

		// iterates over frames	
		for(int i = 0 ; i < nC ; i++)
		{
			MOTDetections dets;
			//
			ch1 = r->GetChildren()[i];		// level 1: "frame" element
			int idx = ch1->FindVariableZ("number")->GetValueInt();
			ch2 = ch1->GetChildren()[0];	// level 2: "objectList" element
			int nO = ch2->GetChildrenNum();		
			for (int j = 0; j < nO; j++)
			{
				ch3 = ch2->GetChildren()[j];	// level 3: "object" element
				double conf;
				if (ch3->FindVariable("confidence") == -1)
					conf = 0.0;
				else 
					conf = ch3->FindVariableZ("confidence")->GetValueFloat();			
				ch4 = ch3->GetChildren()[0];	// level 4: "box" element
				oH = ch4->FindVariableZ("h")->GetValueFloat();
				oW = ch4->FindVariableZ("w")->GetValueFloat();
				oXc = ch4->FindVariableZ("xc")->GetValueFloat();
				oYc = ch4->FindVariableZ("yc")->GetValueFloat();
				//
				Rect bb = Rect((int)(oXc - oW/2), (int)(oYc - oH/2), (int)oW, (int)oH);
				dets.push_back(Detection(bb, conf));
			}		
			// sync
			dr.m_result.push_back(dets);
			dr.m_fIdxes.push_back(idx);
		}
	}
	else
	{
		cout << "This detection result format is not supported!\n";
		delete xmlf;
		return 0;
	}
	delete xmlf;

	cout << "Successfully loaded - " << dr.m_url << endl;	
	return 1;
}

int XMLFileWrapper::writeOut( const TrackBasedResult &result, std::string outFile, int sIdx, int eIdx, int offset )
{
	// root
	XML *xml = new XML();
	XMLElement *r = new XMLElement(0, "Video");
	xml->SetRootElement(r);

	XMLElement *ch1, *ch2; // child node with different depth level
	XMLVariable *v;

	v = new XMLVariable("fname", "mypath\\myvideo"); // really needed ?
	r->AddVariable(v);
	v = new XMLVariable("start_frame");
	v->SetValueInt(sIdx);
	r->AddVariable(v);
	v = new XMLVariable("end_frame");
	v->SetValueInt(eIdx);
	r->AddVariable(v);

	//assert(result.m_nFrames == result.m_result.size());
	int nT = result.m_result.size();
	for (int i = 0; i < nT; i++)
	{
		MOTTrajectory trac = result.m_result[i];
		// "Trajectory" element
		ch1 = r->AddElement("Trajectory");
		v = new XMLVariable("obj_id");
		v->SetValueInt(trac.id);
		ch1->AddVariable(v);
		
		v = new XMLVariable("start_frame");
		v->SetValueInt(trac.idx.sIdx);
		ch1->AddVariable(v);
		v = new XMLVariable("end_frame");
		v->SetValueInt(trac.idx.eIdx);
		ch1->AddVariable(v);
		// "Frame" element
		int n  = (int)trac.trac.size();
		for (int j = 0; j < n; j++)
		{
			ObjTrackInfo &tFrame = trac.trac[j];
			ch2 = ch1->AddElement("Frame");
			// variables
			v = new XMLVariable("frame_no");
			v->SetValueInt(tFrame.fIdx);
			ch2->AddVariable(v);
			v = new XMLVariable("x");
			v->SetValueInt(tFrame.info.bb.x);
			ch2->AddVariable(v);
			v = new XMLVariable("y");
			v->SetValueInt(tFrame.info.bb.y);
			ch2->AddVariable(v);
			v = new XMLVariable("width");
			v->SetValueInt(tFrame.info.bb.width);
			ch2->AddVariable(v);
			v = new XMLVariable("height");
			v->SetValueInt(tFrame.info.bb.height);
			ch2->AddVariable(v);
			// seem fixed variables
			v = new XMLVariable("observation");
			v->SetValueInt(0);
			ch2->AddVariable(v);
			v = new XMLVariable("annotation");
			v->SetValueInt(1);
			ch2->AddVariable(v);
			v = new XMLVariable("contour_pt");
			v->SetValueInt(0);
			ch2->AddVariable(v);
		}
	}
	// XML object save			
	if (xml->Save(outFile.c_str()) == 1)
		cout << "Saved - " << outFile << endl;

	// XML object bye bye
	delete xml;
	return 1;
}

int XMLFileWrapper::writeOut( const FrameBasedResult &result, std::string outFile, int sIdx, int eIdx, int offset /*= 1*/ )
{
	// root
	XML *xml = new XML();
	XMLElement *r = new XMLElement(0, "dataset");
	xml->SetRootElement(r);

	XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
	XMLVariable *v;

	//assert(offset != 0);
	int nF = result.m_result.size();
	//assert((eIdx - sIdx) / offset == nF);
	int idx = sIdx;

	for (int i = 0; i < nF; i++)
	{
		// "frame" element
		ch1 = r->AddElement("frame");
		v = new XMLVariable("number");
		v->SetValueInt(idx);
		ch1->AddVariable(v);
		//idx += offset;
		// "objectList" element
		ch2 = ch1->AddElement("objectlist");
		int n = (int)result.m_result[i].size();
		for (int j = 0; j < n; j++)
		{
			ObjFrameInfo tObj = result.m_result[i][j];
			// "object" element
			ch3 = ch2->AddElement("object");
			v = new XMLVariable("id");
			v->SetValueInt(tObj.id);
			ch3->AddVariable(v);
			// "box" element
			int h = tObj.info.bb.height,
				w = tObj.info.bb.width,
				x = tObj.info.bb.x,
				y = tObj.info.bb.y;
			ch4 = ch3->AddElement("box");
			v = new XMLVariable("h"); // h
			v->SetValueInt(h);
			ch4->AddVariable(v);
			v = new XMLVariable("w"); // w
			v->SetValueInt(w);
			ch4->AddVariable(v);
			v = new XMLVariable("xc"); // xc
			v->SetValueInt(x + w / 2);
			ch4->AddVariable(v);
			v = new XMLVariable("yc"); // yc
			v->SetValueInt(y + h / 2);
			ch4->AddVariable(v);
		}
		//
		idx += offset;
	}
	// XML object save			
	if (xml->Save(outFile.c_str()) == 1)
		cout << "Saved - " << outFile << endl;

	// XML object bye bye
	delete xml;
	return 1;
}

int XMLFileWrapper::writeOut( const DetectionResult &dr, std::string outFile, int mode, int sIdx, int eIdx, int offset /*= 1*/ )
{
	XML *xml = new XML();
	if (mode == 1)	// video format (Bo Yang's)
	{
		XMLElement *r = new XMLElement(0, "Video");
		xml->SetRootElement(r);

		XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
		XMLVariable *v;

		int nF = (int)dr.m_result.size();
		//assert((eIdx - sIdx + 1) / offset == nF);
		int idx = sIdx;

		for (int i = 0; i < nF; i++)
		{
			// "Frame" element
			ch1 = r->AddElement("Frame");
			v = new XMLVariable("no");
			v->SetValueInt(idx);
			ch1->AddVariable(v);
			// "ObjectList" element
			ch2 = ch1->AddElement("ObjectList");
			int n = (int)dr.m_result[i].size();
			for (int j = 0; j < n; j++)
			{
				Detection det = dr.m_result[i][j];
				// "Object" element
				ch3 = ch2->AddElement("Object");
				v = new XMLVariable("id");
				v->SetValueInt(0);
				ch3->AddVariable(v);
				// "Rect" element
				ch4 = ch3->AddElement("Rect");
				int h = det.bb.height,
					w = det.bb.width,
					x = det.bb.x,
					y = det.bb.y;				
				//				
				v = new XMLVariable("x"); // xc
				v->SetValueInt(x);
				ch4->AddVariable(v);
				v = new XMLVariable("y"); // yc
				v->SetValueInt(y);
				ch4->AddVariable(v);
				v = new XMLVariable("height"); // h
				v->SetValueInt(h);
				ch4->AddVariable(v);
				v = new XMLVariable("width"); // w
				v->SetValueInt(w);
				ch4->AddVariable(v);
			}
			//
			idx += offset;
		}		
	}
	else
	{
		XMLElement *r = new XMLElement(0, "dataset");
		xml->SetRootElement(r);

		XMLElement *ch1, *ch2, *ch3, *ch4; // child node with different depth level
		XMLVariable *v;

		int nF = (int)dr.m_result.size();
		assert((eIdx - sIdx + 1) / offset == nF);
		int idx = sIdx;

		for (int i = 0; i < nF; i++)
		{
			// "frame" element
			ch1 = r->AddElement("frame");
			v = new XMLVariable("number");
			v->SetValueInt(idx);
			ch1->AddVariable(v);
			// "objectlist" element
			ch2 = ch1->AddElement("objectlist");
			int n = (int)dr.m_result[i].size();
			for (int j = 0; j < n; j++)
			{
				Detection det = dr.m_result[i][j];
				// "object" element
				ch3 = ch2->AddElement("object");
				v = new XMLVariable("confidence");
				v->SetValueFloat(1.0f);
				ch3->AddVariable(v);
				// "box" element
				ch4 = ch3->AddElement("box");
				int h = det.bb.height,
					w = det.bb.width,
					x = det.bb.x,
					y = det.bb.y;
				//
				v = new XMLVariable("h"); // h
				v->SetValueInt(h);
				ch4->AddVariable(v);
				v = new XMLVariable("w"); // w
				v->SetValueInt(w);
				ch4->AddVariable(v);
				v = new XMLVariable("xc"); // xc
				v->SetValueInt(x + w / 2);
				ch4->AddVariable(v);
				v = new XMLVariable("yc"); // yc
				v->SetValueInt(y + h / 2);
				ch4->AddVariable(v);
			}
			//
			idx += offset;
		}
	}	
	// XML object save			
	if (xml->Save(outFile.c_str()) == 1)
		cout << "Saved - " << outFile << endl;

	// XML object bye bye
	delete xml;
	return 1;
}

bool XMLFileWrapper::fileCheck( std::string fileName )
{
	fstream fs;
	fs.open(fileName.c_str());
	if (!fs.is_open())
	{
		cout << "File not exist exception!\n";
		cout << fileName << " can not be found!\n";		
		return 0;
	}
	fs.close();
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// converter

int XMLFileWrapper::convert( TrackBasedResult &tb, FrameBasedResult &fb )
{
	if (!tb.m_result.empty() && !fb.m_result.empty())	// both not empty
	{
		cout << "Can not decide which to convert!\n";
		return 0;
	}

	if (tb.m_result.empty())	// fb -> tb
	{
		// stats
		tb.m_idx.sIdx = fb.m_idx.sIdx;
		tb.m_idx.eIdx = fb.m_idx.eIdx;
		tb.m_idx.offset = fb.m_idx.offset;
		tb.m_nFrames = fb.m_nFrames;
		tb.m_nTracks = fb.m_nTracks;
		// push in empty trajectories
		//MOTTrajectory trac;
		//for (int i = 0; i < mGT->totalTracs; i++)
		//{
		//	trac.objId = i + 1;	// re-index the objects from 1 to totalObjs
		//	mGT->trajectories.push_back(trac);
		//}
		//// iterates over all frames
		//assert(cvmlGT->nFrames == cvmlGT->frameList.size());
		//for (int i = 0; i < cvmlGT->nFrames; i++)
		//{
		//	MOTTracFrame tempF;
		//	tempF.idx = i + cvmlGT->sIdx;

		//	for (int j = 0; j < (int)cvmlGT->frameList[i].objList.size(); j++)
		//	{
		//		tempF.bb = cvmlGT->frameList[i].objList[j].bb;
		//		int obj_id = cvmlGT->frameList[i].objList[j].id;
		//		// trajectories is 0-based
		//		mGT->trajectories[obj_id - 1].tracFrames.push_back(tempF);	
		//	}
		//}
		//// iterates over all trajectories to fill in those stats
		//int sIdx, eIdx, fNum;
		//for (int i = 0; i < mGT->totalTracs; i++)
		//{
		//	fNum = mGT->trajectories[i].tracFrames.size();
		//	if (fNum > 0)
		//	{
		//		sIdx = mGT->trajectories[i].tracFrames[0].idx;
		//		eIdx = mGT->trajectories[i].tracFrames[fNum - 1].idx;
		//	}
		//	else
		//	{
		//		sIdx = 0;
		//		eIdx = 0;
		//		fNum = 1;
		//	}
		//	mGT->trajectories[i].sIdx = sIdx;
		//	mGT->trajectories[i].eIdx = eIdx;
		//	mGT->trajectories[i].offset = max((eIdx - sIdx + 1) / fNum, 1);
		//	mGT->trajectories[i].missingF = eIdx - sIdx + 1 - fNum;
		//}		
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	else	// tb -> fb
	{
		// stats
		fb.m_idx.sIdx = tb.m_idx.sIdx;
		fb.m_idx.eIdx = tb.m_idx.eIdx;
		fb.m_idx.offset = tb.m_idx.offset;
		fb.m_nFrames = tb.m_nFrames;
		fb.m_nTracks = tb.m_nTracks;
		// push all the empty frames
		MOTOutput op;
		for (int i = 0; i < fb.m_nFrames; i++)
		{
			fb.m_result.push_back(op);
		}
		// iterates over all trajectories
		for (int i = 0; i < tb.m_nTracks; i++)
		{
			MOTTrajectory &traj = tb.m_result[i];
			
			ObjFrameInfo fInfo;
			fInfo.id = traj.id;	// or re-index as i
			fInfo.category = traj.category;

			int nF = (int)traj.trac.size();
			for (int j = 0; j < nF; j++)
			{
				ObjTrackInfo &info = traj.trac[j];
				int fIdx = info.fIdx;
				fInfo.info = info.info;
				if (fIdx < (int)fb.m_result.size())
				{
					fb.m_result[fIdx].push_back(fInfo);
				}
			}
		}
		return 1;
	}
}