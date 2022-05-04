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


class Ini
{
private:
	bool LoadFile(std::string);

public:

	bool Loaded;

	std::vector<std::string> Lines;

	Ini(std::string);

	bool ReadBoolean(std::string, std::string, bool);
	int ReadInteger(std::string, std::string key, int);
	std::string ReadString(std::string, std::string, std::string);	
};