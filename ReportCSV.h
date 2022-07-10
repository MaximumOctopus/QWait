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
	void SaveRideData(const std::string);
	void SaveVisitorData(const std::string);

	void SaveMinuteByMinuteLog(const std::string);

	void SaveRideCount(const std::string);

	void SaveSelectionChoiceCache(const std::string);

	void SaveVisitorLocation(const std::string);

	void SaveVisitorRideList(const std::string);

	void OutputStatus(const std::string);
}