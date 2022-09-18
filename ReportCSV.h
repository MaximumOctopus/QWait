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


#include <fstream>


namespace ReportCSV
{
	void SaveRideData(const std::wstring);
	void SaveVisitorData(const std::wstring);

	void SaveMinuteByMinuteLog(const std::wstring);

	void SaveRideCount(const std::wstring);

	void SaveSelectionChoiceCache(const std::wstring);

	void SaveVisitorLocation(const std::wstring);

	void SaveVisitorRideList(const std::wstring);
	void SaveEateryList(const std::wstring);

	void OutputStatus(const std::wstring);
}