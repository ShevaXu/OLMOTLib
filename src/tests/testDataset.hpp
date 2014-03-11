#include "dataset/datasetconfig.h"

#include <iostream>

using namespace std;

int testSampleConfig(std::string fileName)
{
	DatasetConfig::writeOutSampleXml("output\\" + fileName);
	return 1;
}