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


#include <ctime>
#include <string>
#include "QWaitTypes.h"


static const int kDisplayModeConsole = 0;
static const int kDisplayModeFile = 1;


namespace Utility
{
	std::wstring DateTime(int aMode);

	std::wstring FormatTime(int, int);
	std::wstring FormatTime(QWaitTypes::Time);

	std::wstring PadRight(const std::wstring, int);
	std::wstring PadRight(int, int);

	int Duration(QWaitTypes::Time, QWaitTypes::Time);

	bool IsTimeEqual(QWaitTypes::Time, QWaitTypes::Time);

	bool IsTicketValidForTime(QWaitTypes::Time, QWaitTypes::Time, QWaitTypes::Time);

	std::wstring GetMD5(const std::wstring);
};