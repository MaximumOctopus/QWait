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


#pragma once


#include <string>
#include <vector>


class Names
{
	std::vector<std::string> FirstName;
	std::vector<std::string> LastName;

	void LoadNames();

public:

	bool LoadFirstNames = false;
	bool LoadLastNames = false;

	Names();

	std::string GetRandomName();
};