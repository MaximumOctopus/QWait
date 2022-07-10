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

#include "QWaitTypes.h"
#include "VisitorController.h"


namespace ReportText
{
	void SaveSimulationReport(const std::string, FastPassType);

	void SaveMxMReport(const std::string, FastPassType);

	void MxMRow(std::ofstream& file, MinuteDataV mdv);
	void MxMRowFP(std::ofstream& file, MinuteDataV mdv);

	void OutputStatus(const std::string);
}