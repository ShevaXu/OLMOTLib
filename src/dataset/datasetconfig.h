#ifndef _OLMOT_DATASET_CONFIG_H
#define _OLMOT_DATASET_CONFIG_H

#include <string>
#include <vector>

struct DatasetConfig
{
	DatasetConfig(): m_isLoaded(false) {};

	DatasetConfig(std::string fileName): m_isLoaded(false)
	{
		loadSetting(fileName);
	}

	static void writeOutSampleXml(std::string sampleURL);

	bool loadSetting(std::string fileName);

	void printStats();

	// attributes
	bool m_isLoaded;
	std::string m_datasetName,
		m_dir,
		m_format;
	std::string m_fbGTUrl,
		m_tbGTUrl;
	std::vector<std::string> m_detResUrls;
	int m_sIdx,
		m_eIdx,
		m_offset;
	int m_setting;
	int m_fWidth,
		m_fHeight;
};

#endif // datasetconfig.h