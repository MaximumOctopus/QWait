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


#pragma once


#include <string>
#include <vector>


class Names
{
	std::vector<std::string> FirstName;
	std::vector<std::string> LastName;

	void LoadNames();

public:

	Names();

	std::string GetRandomName();
};

