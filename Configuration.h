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


static const std::string kCats = "/cat";
static const std::string kDebugSCC = "/debugscc";
static const std::string kDebugDistanceCache = "/debugdistancecache";
static const std::string kDegbugNoExecute = "/debugnoexecute";
static const std::string kDebugUpdateRate = "/debugupdaterate";
static const std::string kReportCVD = "/reportcvd";
static const std::string kReportMxMCSV = "/reportcsvmxm";
static const std::string kReportMxMText = "/reporttextmxm";
static const std::string kReportSimulation = "/reportsimulation";
static const std::string kReportSimulationHTML = "/reporthtml";
static const std::string kReportVisitorLocation = "/reportvisitorlocation";
static const std::string kReportVisitorRideList = "/reportvisitorridelist";
static const std::string kImportVisitorDemo = "/importvisitordemo";
static const std::string kReportVisitorDemo = "/reportvisitordemo";
static const std::string kTemplateFile = "/templatefile";
static const std::string kTemplate = "/template";
static const std::string kVisitors = "/visitors";
static const std::string kFastPassMode = "/fastpassmode";
static const std::string kUserConfigFile = "/useconfigfile";
static const std::string kGetVisitorNames = "/getvisitornames";


const static int VisitorCountUseParkAverage = -1;


struct ParameterData {
	std::string command = "";
	std::string property = "";
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

	std::string VisitorDemographicsFile = "";
};


struct TextOutput {
	bool SimulationReport = false;
	bool MinuteByMinute = false;
};


struct HTMLOutput {
	bool SimulationReport = false;
	std::string SimulationReportFile = "";
};


struct SimulationInput {
	bool VisitorDemographics = false;

	std::string VisitorDemographicsFile = "";
};


class Configuration
{
	std::vector<ParameterData> Parameters;

	void SetDefaults();
	void SetFromCommandLine();

	std::string GetCommandValue(const std::string);
	std::string GetPropertyValue(const std::string);

	bool IsValidParameter(const std::string input);

	bool LoadConfigurationFromIni(const std::string);

	std::string SetExtension(const std::string, const std::string);

	void HandleDebugUpdateRate(int);
	void HandleImportVisitorDemo(const std::string);
	void HandleReportVisitorDemo(const std::string);
	void HandleSimulationReportHTML(const std::string);
	void HandleTemplateFile(const std::string);
	void HandleTemplate(const std::string);
	void HandleVisitors(const std::string);
	void HandleFastPassMode(const std::string);
	void HandleUseConfigFile(std::string);

public:

	DebugOutput DebugReports;
	CSVOutput CSVReports;
	HTMLOutput HTMLReports;
	TextOutput TextReports;

	DebugOptions DebugConfig;

	SimulationInput Input;

	int VisitorCount = 0;  // average daily visitors to WDW Florida: Magic Kingdom: 57000; Animal Kingdom: 38000; Epcot: 34000; Hollywood Studios: 31000

	ParkTemplate RideTemplate = ParkTemplate::WDWAnimalKingdom;
	std::string RideTemplateFile = "";

	bool GetVisitorNames = false;

	FastPassType FastPassMode = FastPassType::None;

	int ParkOpenTime = 0;	// 24-hour clock
	int ParkCloseTime = 0;  // 24-hour clock
	int ParkOpenHours = 0;

	Configuration(int argc, char* argv[]);
};