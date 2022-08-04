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
	void SaveSimulationReport(const std::wstring, FastPassType);

	void SaveMxMReport(const std::wstring, FastPassType);

	void MxMRow(std::wofstream& file, MinuteDataV mdv);
	void MxMRowFP(std::wofstream& file, MinuteDataV mdv);

	void OutputStatus(const std::wstring);
}