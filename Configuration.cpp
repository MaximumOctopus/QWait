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


#include <algorithm>
#include <iostream>
#include <string>

#include "Configuration.h"
#include "Constants.h"
#include "Ini.h"
#include "QWaitTypes.h"
#include "Utility.h"


Configuration* GConfiguration;


Configuration::Configuration(int argc, char* argv[])
{
	if (argc != 0)
	{
		for (int t = 1; t < argc; t++)
		{
			std::string parameter = argv[t];

			if (IsValidParameter(parameter))
			{
				ParameterData pd;

				pd.command = GetCommandValue(parameter);
				pd.property = GetPropertyValue(parameter);

				std::transform(pd.command.begin(), pd.command.end(), pd.command.begin(), ::tolower);

				Parameters.push_back(pd);
			}
			else
			{
				std::cerr << "Error, invalid parameter \"" << parameter << "\"." << std::endl;
			}
		}
	}

	SetDefaults();

	SetFromCommandLine();
}


void Configuration::SetDefaults()
{
	RideTemplate = ParkTemplate::WDWAnimalKingdom;
	RideTemplateFile = "";
	CSVReports.VisitorDemographicsFile = "";

	FastPassMode = FastPassType::None;

	VisitorCount = 10000;

	DebugReports.DistanceCache = false;
	DebugReports.SelectionChoiceCache = false;
	DebugConfig.NoExecution = false;
	DebugConfig.DisableConsoleOutput = false;

	CSVReports.MinuteByMinute = false;
	CSVReports.CompleteVisitorData = false;
	CSVReports.VisitorLocation = false;
	CSVReports.VisitorDemographics = false;
	CSVReports.VisitorRideList = false;

	HTMLReports.SimulationReport = false;
	HTMLReports.SimulationReportFile = "";

	TextReports.SimulationReport = false;

	Input.VisitorDemographics = false;
	Input.VisitorDemographicsFile = "";

	ParkOpenTime = 9;
	ParkCloseTime = 22;
	ParkOpenHours = ParkCloseTime - ParkOpenTime;
}


void Configuration::SetFromCommandLine()
{
	if (Parameters.size() != 0)
	{
		for (int p = 0; p < Parameters.size(); p++)
		{
			if (Parameters[p].command.find(kDebugDistanceCache) != std::string::npos)
			{
				DebugReports.DistanceCache = true;
			}
			else if (Parameters[p].command.find(kDebugSCC) != std::string::npos)
			{
				DebugReports.SelectionChoiceCache = true;
			}
			else if (Parameters[p].command.find(kDegbugNoExecute) != std::string::npos)
			{
				DebugConfig.NoExecution = true;
			}
			else if (Parameters[p].command.find(kReportCVD) != std::string::npos)
			{
				CSVReports.CompleteVisitorData = true;
			}
			else if (Parameters[p].command.find(kReportMxMCSV) != std::string::npos)
			{
				CSVReports.MinuteByMinute = true;
			}
			else if (Parameters[p].command.find(kReportMxMText) != std::string::npos)
			{
				TextReports.MinuteByMinute = true;
			}
			else if (Parameters[p].command.find(kReportSimulation) != std::string::npos)
			{
				TextReports.SimulationReport = true;
			}
			else if (Parameters[p].command.find(kReportVisitorLocation) != std::string::npos)
			{
				// be warned! the output file will be around 280MB per 20000 visitors!
				CSVReports.VisitorLocation = true;
			}
			else if (Parameters[p].command.find(kReportVisitorRideList) != std::string::npos)
			{
				CSVReports.VisitorRideList = true;
			}
			else if (Parameters[p].command.find(kGetVisitorNames) != std::string::npos)
			{
				GetVisitorNames = true;
			}
			else if (Parameters[p].command.find(kDebugUpdateRate) != std::string::npos)
			{
				HandleDebugUpdateRate(stoi(Parameters[p].property));
			}
			else if (Parameters[p].command.find(kImportVisitorDemo) != std::string::npos)
			{
				HandleImportVisitorDemo(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kReportSimulationHTML) != std::string::npos)
			{
				HandleSimulationReportHTML(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kReportVisitorDemo) != std::string::npos)
			{
				HandleReportVisitorDemo(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kTemplateFile) != std::string::npos)
			{
				HandleTemplateFile(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kTemplate) != std::string::npos)
			{
				HandleTemplate(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kVisitors) != std::string::npos)
			{
				HandleVisitors(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kFastPassMode) != std::string::npos)
			{
				HandleFastPassMode(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kUserConfigFile) != std::string::npos)
			{
				HandleUseConfigFile(Parameters[p].property);
			}
		}
	}
}


void Configuration::HandleDebugUpdateRate(int update_rate)
{
	if (update_rate > 0)
	{
		DebugConfig.UpdateRate = update_rate;
	}
	else
	{
		std::cerr << "Error, invalid update rate \"" << update_rate << "\". Using 15 (default)." << std::endl;
	}
}


void Configuration::HandleImportVisitorDemo(const std::string file_name)
{
	if (file_name != "")
	{
		Input.VisitorDemographicsFile = SetExtension(file_name, ".csv");

		Input.VisitorDemographics = true;
	}
	else
	{
		std::cerr << "Error, no visitor demo ouput file specified. Using random visitor demographics." << std::endl;
	}
}


void Configuration::HandleSimulationReportHTML(const std::string file_name)
{
	if (file_name != "")
	{
		HTMLReports.SimulationReportFile = SetExtension(file_name, ".html");
	}
	else
	{
		HTMLReports.SimulationReportFile = Utility::DateTime(kDisplayModeFile) + ".html";

		std::cerr << "No html report ouput file specified, using \"" << HTMLReports.SimulationReportFile << "\"." << std::endl;
	}

	HTMLReports.SimulationReport = true;
}


void Configuration::HandleReportVisitorDemo(const std::string file_name)
{
	if (file_name != "")
	{
		CSVReports.VisitorDemographicsFile = SetExtension(file_name, ".csv");

		CSVReports.VisitorDemographics = true;
	}
	else
	{
		std::cerr << "Error, no visitor demo ouput file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplateFile(const std::string file_name)
{
	if (file_name != "")
	{
		RideTemplateFile = SetExtension(file_name, ".template");
	}
	else
	{
		std::cerr << "Error, no template file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplate(const std::string property)
{
	if (property != "")
	{
		try
		{
			int ridetemplate = std::stoi(property);

			if (ridetemplate < 0 || ridetemplate > Constants::TemplateMaximum)
			{
				RideTemplate = ParkTemplate::WDWAnimalKingdom;
			}
			else
			{
				RideTemplate = QWaitTypes::IntToParkTemplate(ridetemplate);
			}
		}
		catch (...)
		{
			std::cerr << "Invalid template ID parameter: " << property << std::endl;
		}
	}
	else
	{
		std::cerr << "Error, no template specified. Using 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleVisitors(const std::string property)
{
	if (property != "")
	{
		try
		{
			VisitorCount = std::stoi(property);
		}
		catch (...)
		{
			std::cout << "Invalid visitor count parameter: " << property << std::endl;
		}
	}
	else
	{
		std::cerr << "Error, no visitor count specified. Using 10000." << std::endl;
	}
}


void Configuration::HandleFastPassMode(const std::string property)
{
	if (property != "")
	{
		try
		{
			int fpm = std::stoi(property);

			FastPassMode = QWaitTypes::IntToFastPassMode(fpm);
		}
		catch (...)
		{
			std::cout << "Invalid FastPass mode parameter: " << property << std::endl;
		}
	}
	else
	{
		std::cerr << "Error, no FastPass mode specified. Using 0 (none)." << std::endl;
	}
}


void Configuration::HandleUseConfigFile(std::string file_name)
{
	if (file_name == "")
	{
		file_name = "QWaitConfig.ini";
	}
	else
	{
		file_name = SetExtension(file_name, ".ini");
	}

	if (!LoadConfigurationFromIni(file_name))
	{
		std::cerr << "Error, couldn't load config \"" + file_name + "\". Ignored." << std::endl;
	}
}


std::string Configuration::GetCommandValue(const std::string input)
{
	size_t pos = input.find(';');

	if (pos != std::string::npos)
	{
		return input.substr(0, pos);
	}

	return input;
}


std::string Configuration::GetPropertyValue(const std::string input)
{
	size_t pos = input.find(';');

	if (pos != std::string::npos)
	{
		return input.substr(pos + 1);
	}

	return "";
}


bool Configuration::IsValidParameter(const std::string input)
{
	std::string parameter = input;

	std::transform(parameter.begin(), parameter.end(), parameter.begin(), ::tolower);

	if (parameter == kCats)
	{
		return true;
	}
	else if (parameter == kDebugSCC)
	{
		return true;
	}
	else if (parameter == kDebugDistanceCache)
	{
		return true;
	}
	else if (parameter == kDegbugNoExecute)
	{
		return true;
	}
	else if (parameter == kReportCVD)
	{
		return true;
	}
	else if (parameter == kReportMxMCSV)
	{
		return true;
	}
	else if (parameter == kReportMxMText)
	{
		return true;
	}
	else if (parameter == kReportSimulation)
	{
		return true;
	}
	else if (parameter == kReportVisitorLocation)
	{
		return true;
	}
	else if (parameter == kGetVisitorNames)
	{
		return true;
	}
	else if (parameter.find(kDebugUpdateRate) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kImportVisitorDemo) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kReportSimulationHTML) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kReportVisitorDemo) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kReportVisitorRideList) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kTemplateFile) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kTemplate) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kVisitors) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kFastPassMode) != std::string::npos)
	{
		return true;
	}
	else if (parameter.find(kUserConfigFile) != std::string::npos)
	{
		return true;
	}

	return false;
}


std::string Configuration::SetExtension(const std::string file_name, const std::string extension)
{
	if (file_name.find(extension) == std::string::npos)
	{
		return file_name + extension;
	}

	return file_name;
}


bool Configuration::LoadConfigurationFromIni(const std::string file_name)
{
	Ini* config = new Ini(file_name);

	bool isError = false;

	if (config->Loaded)
	{
		// == operation ==========================================================================================

		int vc = config->ReadInteger("main", "VisitorCount", 0);
		int rt = config->ReadInteger("main", "RideTemplate", -1);
		int fm = config->ReadInteger("main", "FastPassMode", 0);

		int po = config->ReadInteger("main", "ParkOpenTime", 9);
		int pc = config->ReadInteger("main", "ParkCloseTime", 22);

		// == reports ============================================================================================

		DebugReports.DistanceCache = config->ReadBoolean("reports", "DebugDistanceCache", false);
		DebugReports.SelectionChoiceCache = config->ReadBoolean("reports", "DebugSelectionChoiceCache", false);
		DebugConfig.UpdateRate = config->ReadInteger("reports", "DebugUpdateRate", 15);
		DebugConfig.NoExecution = config->ReadBoolean("reports", "DebugNoExecution", false);

		CSVReports.MinuteByMinute = config->ReadBoolean("reports", "ReportMinuteByMinute", false);
		CSVReports.CompleteVisitorData = config->ReadBoolean("reports", "ReportCompleteVisitorData", false);
		CSVReports.VisitorLocation = config->ReadBoolean("reports", "ReportVisitorLocation", false);
		CSVReports.VisitorRideList = config->ReadBoolean("reports", "ReportVisitorRideList", false);

		HTMLReports.SimulationReport = config->ReadBoolean("reports", "ReportHTML", false);

		TextReports.MinuteByMinute = config->ReadBoolean("reports", "ReportTextMXM", false);
		TextReports.SimulationReport = config->ReadBoolean("reports", "SimulationReport", false);

		// =======================================================================================================

		if (vc == 0 || rt == -1)
		{
			std::cerr << "VisitorCount cannot be zero / RideTemplate must be between 0 and " << Constants::TemplateMaximum << "." << std::endl;

			isError = true;
		}
		else if (pc <= po)
		{
			std::cerr << "Park close time must be after park open time." << std::endl;

			isError = true;
		}
		else if (fm >= 0 && fm <= 2)
		{
			std::cerr << "Invalid FastPassMode (must be between " << 0 << " and " << 2 << ")." << std::endl;

			isError = true;
		}
		else
		{
			VisitorCount  = vc;
			RideTemplate  = QWaitTypes::IntToParkTemplate(rt);
			FastPassMode  = QWaitTypes::IntToFastPassMode(fm);
			ParkOpenTime  = po;
			ParkCloseTime = pc;
		}

		ParkOpenHours = ParkCloseTime - ParkOpenTime;
	}
	else
	{
		std::cerr << "Error loading config file." << std::endl;

		isError = true;
	}

	delete config;

	return !isError;
}