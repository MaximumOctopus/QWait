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

	void OutputStatus(const std::string);

public:

	ReportController(bool);

	// =======================================================================
    // == CSV Reports =======================================================
    // =======================================================================

	void SaveRideDataCSV(const std::string);
	void SaveVisitorDataCSV(const std::string);
	void SaveMinuteByMinuteLogCSV(const std::string);

	void SaveRideCountCSV(const std::string);

	void SaveSelectionChoiceCacheCSV(const std::string);

	void SaveVisitorLocationCSV(const std::string);

	void SaveVisitorRideListCSV(const std::string);

	// =======================================================================
	// == HTML Reports =======================================================
	// =======================================================================

	void SaveSimulationReportHTML(const std::string, FastPassType);

	// =======================================================================
	// == Text Reports =======================================================
	// =======================================================================

	void SaveMXMReportText(const std::string, FastPassType);
	void SaveSimulationReportText(const std::string, FastPassType);	
};