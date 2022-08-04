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


class ReportController
{
	bool ShowOutput = true;

	void OutputStatus(const std::wstring);

public:

	ReportController(bool);

	// =======================================================================
    // == CSV Reports =======================================================
    // =======================================================================

	void SaveRideDataCSV(const std::wstring);
	void SaveVisitorDataCSV(const std::wstring);
	void SaveMinuteByMinuteLogCSV(const std::wstring);

	void SaveRideCountCSV(const std::wstring);

	void SaveSelectionChoiceCacheCSV(const std::wstring);

	void SaveVisitorLocationCSV(const std::wstring);

	void SaveVisitorRideListCSV(const std::wstring);

	// =======================================================================
	// == HTML Reports =======================================================
	// =======================================================================

	void SaveSimulationReportHTML(const std::wstring, FastPassType);

	// =======================================================================
	// == Text Reports =======================================================
	// =======================================================================

	void SaveMXMReportText(const std::wstring, FastPassType);
	void SaveSimulationReportText(const std::wstring, FastPassType);	
};