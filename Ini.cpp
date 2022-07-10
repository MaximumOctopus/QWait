// =======================================================================
//
// QWait, theme park modeller
//
// (c) Paul Alan Freshney 2022
// paul@freshney.org
//
// https://github.com/MaximumOctopus/QWait
// 
// =======================================================================


#include "Ini.h"

#include <algorithm>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <vector>


Ini::Ini(const std::string file_name)
{
	Loaded = LoadFile(file_name);
}


bool Ini::LoadFile(const std::string file_name)
{
	std::ifstream file(file_name);

	if (file)
	{
		Lines.clear();

		std::string s;

		while (std::getline(file, s))
		{
			if (s != "")
			{
				Lines.push_back(s);
			}
		}

		file.close();

		return true;
	}

	return false;
}


int Ini::ReadInteger(const std::string section, const std::string key, int default_value)
{
	try
	{
		int i = std::stoi(ReadString(section, key, "-1"));

		if (i == -1)
		{
			return default_value;
		}

		return i;
	}
	catch(...)
	{
		std::cerr << "Invalid integer parameter \"" << key << "\" in [" << section << "]." << section << std::endl;

		return default_value;
	}
}


bool Ini::ReadBoolean(const std::string section, const std::string key, bool default_value)
{
	try
	{
		int i = std::stoi(ReadString(section, key, "-1"));

		if (i == -1)
		{
			return default_value;
		}
		else if (i == 0)
		{
			return false;
		}

		return true;
	}
	catch (...)
	{
		std::cerr << "Invalid boolean parameter \"" << key << "\" in [" << section << "]." << section << std::endl;

		return default_value;
	}
}


std::string Ini::ReadString(std::string section, std::string key, const std::string default_value)
{
	bool inSection = false;
	std::string sectionName = "";

	key = key + '=';

	std::transform(section.begin(), section.end(), section.begin(), ::tolower);

	for (int i = 0; i < Lines.size(); i++)
	{
		if (Lines[i][0] == '[')
		{
			if (inSection)
			{
				// we were in the right section, but now entered a new one.... which means the key we're looking
				// for must be missing, so return the default value

				return default_value;
			}
			else
			{
				std::size_t bo = Lines[i].find("[");
				std::size_t bc = Lines[i].find("]");

				sectionName = Lines[i].substr(bo + 1, bc - 1);

				std::transform(sectionName.begin(), sectionName.end(), sectionName.begin(), ::tolower);

				if (sectionName == section)
				{
					inSection = true;
				}
			}
		}
		else
		{
			if (inSection)
			{
				if (Lines[i] != "")
				{
					if ((Lines[i][0] != ';') && (Lines[i][0] != '/'))
					{
						if (Lines[i].find(key) == 0)
						{
							if (Lines[i].length() > key.length())
							{
								return Lines[i].substr(key.length());
							}
							else
							{
								// key is emtpy (key= ) so let's return the default                                

								return default_value;
							}
						}
					}
				}
			}
		}
	}

	return default_value;
}