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


class ReportController
{
	bool ShowOutput;

	void OutputStatus(std::string);

public:

	ReportController(bool);

	// =======================================================================
	// == CSV Reports ========================================================
    // =======================================================================

	void SaveRideDataCSV(std::string);
	void SaveVisitorDataCSV(std::string);
	void SaveMinuteByMinuteLogCSV(std::string);

	void SaveRideCountCSV(std::string);

	void SaveSelectionChoiceCacheCSV(std::string);

	void SaveVisitorLocationCSV(std::string);

	// =======================================================================
	// == Text Reports =======================================================
	// =======================================================================

	void SaveSimulationReport(std::string, int);

	// =======================================================================
	// == HTML Reports =======================================================
	// =======================================================================
	
	void SaveSimulationReportHTML(std::string, int);
};