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

// source
// https://www.ocregister.com/2020/05/22/what-an-attendance-cap-could-mean-for-crowm


#pragma once


#include <string>
#include <vector>

#include "Constants.h"
#include "QWaitTypes.h"


static const std::wstring kCats = L"/cat";
static const std::wstring kDebugSCC = L"/debugscc";
static const std::wstring kDebugDistanceCache = L"/debugdistancecache";
static const std::wstring kDegbugNoExecute = L"/debugnoexecute";
static const std::wstring kDebugUpdateRate = L"/debugupdaterate";
static const std::wstring kReportCVD = L"/reportcvd";
static const std::wstring kReportMxMCSV = L"/reportcsvmxm";
static const std::wstring kReportMxMText = L"/reporttextmxm";
static const std::wstring kReportSimulation = L"/reportsimulation";
static const std::wstring kReportSimulationHTML = L"/reporthtml";
static const std::wstring kReportVisitorLocation = L"/reportvisitorlocation";
static const std::wstring kReportVisitorRideList = L"/reportvisitorridelist";
static const std::wstring kImportVisitorDemo = L"/importvisitordemo";
static const std::wstring kReportVisitorDemo = L"/reportvisitordemo";
static const std::wstring kTemplateFile = L"/templatefile";
static const std::wstring kTemplate = L"/template";
static const std::wstring kVisitors = L"/visitors";
static const std::wstring kFastPassMode = L"/fastpassmode";
static const std::wstring kUserConfigFile = L"/useconfigfile";
static const std::wstring kGetVisitorNames = L"/getvisitornames";


const static int VisitorCountUseParkAverage = -1;


struct ParameterData {
	std::wstring command = L"";
	std::wstring property = L"";
};


struct DebugOutput {
	bool DistanceCache = false;
	bool SelectionChoiceCache = false;
};


struct DebugOptions {
	bool NoExecution = false;
	bool DisableConsoleOutput = false;

	int UpdateRate = 15;
};


struct CSVOutput {
	bool MinuteByMinute = false;
	bool CompleteVisitorData = false;
	bool VisitorLocation = false;
	bool VisitorDemographics = false;
	bool VisitorRideList = false;

	std::wstring VisitorDemographicsFile = L"";
};


struct TextOutput {
	bool SimulationReport = false;
	bool MinuteByMinute = false;
};


struct HTMLOutput {
	bool SimulationReport = false;
	std::wstring SimulationReportFile = L"";
};


struct SimulationInput {
	bool VisitorDemographics = false;

	std::wstring VisitorDemographicsFile = L"";
};


class Configuration
{
	std::vector<ParameterData> Parameters;

	void SetDefaults();
	void SetFromCommandLine();

	std::wstring GetCommandValue(const std::wstring);
	std::wstring GetPropertyValue(const std::wstring);

	bool IsValidParameter(const std::wstring input);

	bool LoadConfigurationFromIni(const std::wstring);

	std::wstring SetExtension(const std::wstring, const std::wstring);

	void HandleDebugUpdateRate(int);
	void HandleImportVisitorDemo(const std::wstring);
	void HandleReportVisitorDemo(const std::wstring);
	void HandleSimulationReportHTML(const std::wstring);
	void HandleTemplateFile(const std::wstring);
	void HandleTemplate(const std::wstring);
	void HandleVisitors(const std::wstring);
	void HandleFastPassMode(const std::wstring);
	void HandleUseConfigFile(std::wstring);

public:

	DebugOutput DebugReports;
	CSVOutput CSVReports;
	HTMLOutput HTMLReports;
	TextOutput TextReports;

	DebugOptions DebugConfig;

	SimulationInput Input;

	int VisitorCount = 0;  // average daily visitors to WDW Florida: Magic Kingdom: 57000; Animal Kingdom: 38000; Epcot: 34000; Hollywood Studios: 31000

	ParkTemplate RideTemplate = ParkTemplate::WDWAnimalKingdom;
	std::wstring RideTemplateFile = L"";

	bool GetVisitorNames = false;

	FastPassType FastPassMode = FastPassType::None;

	int ParkOpenTime = 0;	// 24-hour clock
	int ParkCloseTime = 0;  // 24-hour clock
	int ParkOpenHours = 0;

	Configuration(int argc, wchar_t* argv[]);
};