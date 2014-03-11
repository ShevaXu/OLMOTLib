#include "../io/player.h"
#include "../dataset/datasetconfig.h"

#include <iostream>

#pragma once

using namespace std;

inline void dataset2ImgSeq(const DatasetConfig &dataset, ImgSeqConfig &conf)
{
	conf.dir = dataset.m_dir;
	conf.sIdx = dataset.m_sIdx;
	conf.eIdx = dataset.m_eIdx;
	conf.offset = dataset.m_offset;
	conf.fileName = dataset.m_format;
	conf.frames = (conf.eIdx - conf.sIdx + 1) / conf.offset;
}