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


class Ini
{
private:
	bool LoadFile(const std::wstring);

public:

	bool Loaded;

	std::vector<std::wstring> Lines;

	Ini(const std::wstring);

	bool ReadBoolean(const std::wstring, const std::wstring, bool);
	int ReadInteger(const std::wstring, const std::wstring key, int);
	std::wstring ReadString(std::wstring, std::wstring, const std::wstring);
};