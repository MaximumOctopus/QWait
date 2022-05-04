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

// source
// https://www.ocregister.com/2020/05/22/what-an-attendance-cap-could-mean-for-crowded-disney-parks-in-the-covid-19-era/


#pragma once


#include <string>
#include <vector>


static const std::string kDebugSCC = "/debugscc";
static const std::string kDegbugNoExecute = "/debugnoexecute";
static const std::string kReportCVD = "/reportcvd";
static const std::string kReportMxM = "/reportmxm";
static const std::string kReportSimulation = "/reportsimulation";
static const std::string kReportSimulationHTML = "/reporthtml";
static const std::string kReportVisitorLocation = "/reportvisitorlocation";
static const std::string kImportVisitorDemo = "/importvisitordemo";
static const std::string kReportVisitorDemo = "/reportvisitordemo";
static const std::string kTemplateFile = "/templatefile";
static const std::string kTemplate = "/template";
static const std::string kVisitors = "/visitors";
static const std::string kFastPassMode = "/fastpassmode";
static const std::string kUserConfigFile = "/useconfigfile";


const static int VisitorCountUseParkAverage = -1;


struct ParameterData {
	std::string command = "";
	std::string property = "";
};


struct DebugOutput {
	bool SelectionChoiceCache = false;
};


struct DebugOptions {
	bool NoExecution = false;
	bool DisableConsoleOutput = false;
};


struct CSVOutput {
	bool MinuteByMinute = false;
	bool CompleteVisitorData = false;
	bool VisitorLocation = false;
	bool VisitorDemographics = false;

	std::string VisitorDemographicsFile = "";
};


struct TextOutput {
	bool SimulationReport = false;
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

	std::string GetCommandValue(std::string);
	std::string GetPropertyValue(std::string);

	bool IsValidParameter(std::string input);

	bool LoadConfigurationFromIni(std::string);

	std::string SetExtension(std::string, std::string);

	void HandleImportVisitorDemo(std::string);
	void HandleReportVisitorDemo(std::string);
	void HandleSimulationReportHTML(std::string);
	void HandleTemplateFile(std::string);
	void HandleTemplate(std::string);
	void HandleVisitors(std::string);
	void HandleFastPassMode(std::string);
	void HandleUseConfigFile(std::string);

public:

	DebugOutput DebugReports;
	CSVOutput CSVReports;
	HTMLOutput HTMLReports;
	TextOutput TextReports;

	DebugOptions DebugConfig;

	SimulationInput Input;

	int VisitorCount;  // average daily visitors to WDW Florida: Magic Kingdom: 57000; Animal Kingdom: 38000; Epcot: 34000; Hollywood Studios: 31000

	int RideTemplate;
	std::string RideTemplateFile;

	int FastPassMode;

	int ParkOpenTime;	// 24-hour clock
	int ParkCloseTime;  // 24-hour clock
	int ParkOpenHours;

	Configuration(int argc, char* argv[]);
};