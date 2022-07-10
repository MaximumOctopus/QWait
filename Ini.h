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
	bool LoadFile(const std::string);

public:

	bool Loaded;

	std::vector<std::string> Lines;

	Ini(const std::string);

	bool ReadBoolean(const std::string, const std::string, bool);
	int ReadInteger(const std::string, const std::string key, int);
	std::string ReadString(std::string, std::string, const std::string);
};