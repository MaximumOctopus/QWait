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


#include <fstream>
#include <iostream>

#include "ParkController.h"
#include "ReportController.h"
#include "ReportCSV.h"
#include "ReportHTML.h"
#include "ReportText.h"
#include "Utility.h"
#include "VisitorController.h"


extern ParkController* GParkController;
extern VisitorController* GVisitorController;


ReportController* GReportController;


ReportController::ReportController(bool show_output)
{
	ShowOutput = show_output;
}


void ReportController::OutputStatus(const std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


// =======================================================================================================================
// =======================================================================================================================


void ReportController::SaveRideDataCSV(const std::string file_name)
{
	ReportCSV::SaveRideData(file_name);
}


void ReportController::SaveVisitorDataCSV(const std::string file_name)
{
	ReportCSV::SaveVisitorData(file_name);
}


void ReportController::SaveMinuteByMinuteLogCSV(const std::string file_name)
{
	ReportCSV::SaveMinuteByMinuteLog(file_name);
}


void ReportController::SaveRideCountCSV(const std::string file_name)
{
	ReportCSV::SaveRideCount(file_name);
}


void ReportController::SaveSelectionChoiceCacheCSV(const std::string file_name)
{
	ReportCSV::SaveSelectionChoiceCache(file_name);
}


void ReportController::SaveVisitorLocationCSV(const std::string file_name)
{
	ReportCSV::SaveVisitorLocation(file_name);
}


void ReportController::SaveVisitorRideListCSV(const std::string file_name)
{
	ReportCSV::SaveVisitorRideList(file_name);
}


// =======================================================================================================================
// =======================================================================================================================


void ReportController::SaveSimulationReportHTML(const std::string file_name, FastPassType fastpass_mode)
{
	if (!ReportHTML::SaveSimulationReport(file_name, fastpass_mode))
	{
		std::cerr << "Unable to save HTML Simulation report." << std::endl;
	}
}


// =======================================================================================================================
// =======================================================================================================================


void ReportController::SaveMXMReportText(const std::string file_name, FastPassType fastpass_mode)
{
	ReportText::SaveMxMReport(file_name, fastpass_mode);
}


void ReportController::SaveSimulationReportText(const std::string file_name, FastPassType fastpass_mode)
{
	ReportText::SaveSimulationReport(file_name, fastpass_mode);
}