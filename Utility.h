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


#include <ctime>
#include <string>
#include "QWaitTypes.h"


namespace Utility
{
	std::string DateTime(int aMode);

	std::string FormatTime(int, int);
	std::string FormatTime(QWaitTypes::Time);

	std::string PadRight(std::string, int);
	std::string PadRight(int, int);

	int Duration(QWaitTypes::Time, QWaitTypes::Time);

	bool IsTimeEqual(QWaitTypes::Time, QWaitTypes::Time);

	bool IsTicketValidForTime(QWaitTypes::Time, QWaitTypes::Time, QWaitTypes::Time);

	std::string GetMD5(std::string);
};