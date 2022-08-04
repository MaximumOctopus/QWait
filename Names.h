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
	std::vector<std::wstring> FirstName;
	std::vector<std::wstring> LastName;

	void LoadNames();

public:

	bool LoadFirstNames = false;
	bool LoadLastNames = false;

	Names();

	std::wstring GetRandomName();
};