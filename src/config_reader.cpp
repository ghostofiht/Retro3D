#include "config_reader.h"

#include <fstream>
#include <sstream>
#include "st_assert.h"

namespace Retro3D
{
	bool ConfigReader::ReadFile(const char* arg_filepath)
	{
		std::ifstream configFile(arg_filepath);
		if (!configFile.is_open())
			return false;

		/**
		* Parse config file.
		*/
		std::string currLine;
		std::string currSection;
		while (std::getline(configFile, currLine))
		{
			const char* cstrPtr = currLine.c_str();
			const size_t& len = currLine.size();
			size_t iLeft = 0;
			size_t iRight = 0;
			bool isSection = false;
			bool isVariableAssignment = false;
			std::string currVarName;
			while (iRight < len)
			{
				const char* currChar = cstrPtr + iRight;
				if (isSection)
				{
					if (*currChar == ']')
					{
						currSection = currLine.substr(iLeft, iRight - iLeft);
						break;
					}
				}
				else
				{
					if (*currChar == '[')
					{
						isSection = true;
						iLeft = iRight + 1;
					}
					if (*currChar == ':')
					{
						isVariableAssignment = true;
						currVarName = currLine.substr(iLeft, iRight - iLeft);
						iLeft = iRight + 1;
					}
				}

				iRight++;
			}

			if (isVariableAssignment && iLeft < iRight)
			{
				std::string varValue = currLine.substr(iLeft, 1 + iRight - iLeft);
				mSectionVarMap[currSection][currVarName] = varValue;
			}
		}
	}

	std::unordered_map<std::string, std::string> ConfigReader::GetSectionAsMap(const char* arg_section)
	{
		auto sectionIter = mSectionVarMap.find(arg_section);
		if (sectionIter != mSectionVarMap.end())
			return (*sectionIter).second;
		else
			return std::unordered_map<std::string, std::string>();
	}

	bool ConfigReader::GetString(const char* arg_section, const char* arg_variable, std::string& out_value)
	{
		auto sectionIter = mSectionVarMap.find(arg_section);
		if (sectionIter != mSectionVarMap.end())
		{
			auto varIter = (*sectionIter).second.find(arg_variable);
			if (varIter != (*sectionIter).second.end())
			{
				out_value = (*varIter).second;
				return true;
			}
		}
		return false;
	}

	bool ConfigReader::GetInt(const char* arg_section, const char* arg_variable, int& out_value)
	{
		auto sectionIter = mSectionVarMap.find(arg_section);
		if (sectionIter != mSectionVarMap.end())
		{
			auto varIter = (*sectionIter).second.find(arg_variable);
			if (varIter != (*sectionIter).second.end())
			{
				const std::string& varStr = (*varIter).second;
				try
				{
					out_value = std::atoi(varStr.c_str());
					return true;
				}
				catch (std::invalid_argument ex) {}
			}
		}
		return false;
	}
}