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


#include <fstream>
#include <iostream>
#include <string>
#include "Names.h"


Names::Names()
{
	LoadNames();
}


void Names::LoadNames()
{
	std::wifstream file(L"firstnames.txt");

	if (file)
	{
		std::wstring s;

		std::wcout << L"Loading \"firstnames.txt\"..." << std::endl;

		while (std::getline(file, s))
		{
			if (s != L"")
			{
				FirstName.push_back(s);
			}
		}

		file.close();

		LoadFirstNames = true;
	}
	else
	{
		std::wcout << L"Error loading \"firstnames.txt\"." << std::endl;
	}

	std::wifstream file2(L"surnames.txt");

	if (file2)
	{
		std::wstring s;

		std::wcout << L"Loading \"surnames.txt\"..." << std::endl;

		while (std::getline(file2, s))
		{
			if (s != L"")
			{
				LastName.push_back(s);
			}
		}

		file2.close();

		LoadLastNames = true;
	}
	else
	{
		std::wcout << L"Error loading \"surnames.txt\"." << std::endl;
	}
}


std::wstring Names::GetRandomName()
{
	int first = rand() % FirstName.size();
	int last = rand() % LastName.size();

	return FirstName[first] + L" " + LastName[last];
}