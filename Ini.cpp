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


Ini::Ini(const std::wstring file_name)
{
	Loaded = LoadFile(file_name);
}


bool Ini::LoadFile(const std::wstring file_name)
{
	std::wifstream file(file_name);

	if (file)
	{
		Lines.clear();

		std::wstring s;

		while (std::getline(file, s))
		{
			if (s != L"")
			{
				Lines.push_back(s);
			}
		}

		file.close();

		return true;
	}

	return false;
}


int Ini::ReadInteger(const std::wstring section, const std::wstring key, int default_value)
{
	try
	{
		int i = std::stoi(ReadString(section, key, L"-1"));

		if (i == -1)
		{
			return default_value;
		}

		return i;
	}
	catch(...)
	{
		std::wcerr << L"Invalid integer parameter \"" << key << L"\" in [" << section << L"]." << section << std::endl;

		return default_value;
	}
}


bool Ini::ReadBoolean(const std::wstring section, const std::wstring key, bool default_value)
{
	try
	{
		int i = std::stoi(ReadString(section, key, L"-1"));

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
		std::wcerr << L"Invalid boolean parameter \"" << key << L"\" in [" << section << L"]." << section << std::endl;

		return default_value;
	}
}


std::wstring Ini::ReadString(std::wstring section, std::wstring key, const std::wstring default_value)
{
	bool inSection = false;
	std::wstring sectionName = L"";

	key = key + L'=';

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
				std::size_t bo = Lines[i].find(L"[");
				std::size_t bc = Lines[i].find(L"]");

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
				if (Lines[i] != L"")
				{
					if ((Lines[i][0] != L';') && (Lines[i][0] != L'/'))
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