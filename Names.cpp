// =======================================================================
//
// QWait, theme park modeller
//
// (c) Paul Alan Freshney 2022
// paul@freshney.org
//
// https://qwait.sourceforge.io
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
	std::ifstream file("firstnames.txt");

	if (file)
	{
		std::string s;

		std::cout << "Loading firstnames.txt..." << std::endl;

		while (std::getline(file, s))
		{
			if (s != "")
			{
				FirstName.push_back(s);
			}
		}

		file.close();
	}

	std::ifstream file2("surnames.txt");

	if (file2)
	{
		std::string s;

		std::cout << "Loading surnames.txt..." << std::endl;

		while (std::getline(file2, s))
		{
			if (s != "")
			{
				LastName.push_back(s);
			}
		}

		file2.close();
	}
}


std::string Names::GetRandomName()
{
	int first = rand() % FirstName.size();
	int last  = rand() % LastName.size();

	return FirstName[first] + " " + LastName[last];
}