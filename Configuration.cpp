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


#include <algorithm>
#include <iostream>
#include <string>
#include "Configuration.h"
#include "Constants.h"
#include "Ini.h"


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
	RideTemplate = Constants::TemplateWDWAnimalKingdom;
	RideTemplateFile = "";
	CSVReports.VisitorDemographicsFile = "";

	FastPassMode = Constants::FastPassModeNone;

	VisitorCount = 10000;

	DebugReports.SelectionChoiceCache = false;
	DebugConfig.NoExecution = false;
	DebugConfig.DisableConsoleOutput = false;

	CSVReports.MinuteByMinute = false;
	CSVReports.CompleteVisitorData = false;
	CSVReports.VisitorLocation = false;
	CSVReports.VisitorDemographics = false;

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
			if (Parameters[p].command.find(kDebugSCC) != std::string::npos)
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
			else if (Parameters[p].command.find(kReportMxM) != std::string::npos)
			{
				CSVReports.MinuteByMinute = true;
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


void Configuration::HandleImportVisitorDemo(std::string filename)
{
	if (filename != "")
	{
		Input.VisitorDemographicsFile = SetExtension(filename, ".csv");

		Input.VisitorDemographics = true;
	}
	else
	{
		std::cerr << "Error, no visitor demo ouput file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleSimulationReportHTML(std::string filename)
{
	if (filename != "")
	{
		HTMLReports.SimulationReportFile = SetExtension(filename, ".html");

		HTMLReports.SimulationReport = true;
	}
	else
	{
		std::cerr << "Error, no html report ouput file specified." << std::endl;
	}
}


void Configuration::HandleReportVisitorDemo(std::string filename)
{
	if (filename != "")
	{
		CSVReports.VisitorDemographicsFile = SetExtension(filename, ".csv");

		CSVReports.VisitorDemographics = true;
	}
	else
	{
		std::cerr << "Error, no visitor demo ouput file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplateFile(std::string filename)
{
	if (filename != "")
	{
		RideTemplateFile = SetExtension(filename, ".template");
	}
	else
	{
		std::cerr << "Error, no template file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplate(std::string property)
{
	if (property != "")
	{
		try
		{
			RideTemplate = std::stoi(property);
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


void Configuration::HandleVisitors(std::string property)
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


void Configuration::HandleFastPassMode(std::string property)
{
	if (property != "")
	{
		try
		{
			FastPassMode = std::stoi(property);
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


void Configuration::HandleUseConfigFile(std::string filename)
{
	if (filename == "")
	{
		filename = "QWaitConfig.ini";
	}
	else
	{
		filename = SetExtension(filename, ".ini");
	}

	if (!LoadConfigurationFromIni(filename))
	{
		std::cerr << "Error, couldn't load config \"" + filename + "\". Ignored." << std::endl;
	}
}


std::string Configuration::GetCommandValue(std::string input)
{
	size_t pos = input.find(';');

	if (pos != std::string::npos)
	{
		return input.substr(0, pos);
	}

	return input;
}


std::string Configuration::GetPropertyValue(std::string input)
{
	size_t pos = input.find(';');

	if (pos != std::string::npos)
	{
		return input.substr(pos + 1);
	}

	return "";
}


bool Configuration::IsValidParameter(std::string input)
{
	if (input == kDebugSCC)
	{
		return true;
	}
	else if (input == kDegbugNoExecute)
	{
		return true;
	}
	else if (input == kReportCVD)
	{
		return true;
	}
	else if (input == kReportMxM)
	{
		return true;
	}
	else if (input == kReportSimulation)
	{
		return true;
	}
	else if (input == kReportVisitorLocation)
	{
		return true;
	}
	else if (input.find(kImportVisitorDemo) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kReportSimulationHTML) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kReportVisitorDemo) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kTemplateFile) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kTemplate) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kVisitors) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kFastPassMode) != std::string::npos)
	{
		return true;
	}
	else if (input.find(kUserConfigFile) != std::string::npos)
	{
		return true;
	}

	return false;
}


std::string Configuration::SetExtension(std::string filename, std::string extension)
{
	if (filename.find(extension) == std::string::npos)
	{
		return filename + extension;
	}

	return filename;
}


bool Configuration::LoadConfigurationFromIni(std::string filename)
{
	Ini* config = new Ini("QWaitConfig.ini");

	bool isError = false;

	if (config->Loaded)
	{
		// == operation ==========================================================================================

		int vc = config->ReadInteger("main", "VisitorCount", 0);
		int rt = config->ReadInteger("main", "RideTemplate", -1);
		int fm = config->ReadInteger("main", "FastPassMode", Constants::FastPassModeNone);

		int po = config->ReadInteger("main", "ParkOpenTime", 9);
		int pc = config->ReadInteger("main", "ParkCloseTime", 22);

		// == reports ============================================================================================

		DebugReports.SelectionChoiceCache = config->ReadBoolean("reports", "DebugSelectionChoiceCache", false);
		DebugConfig.NoExecution = config->ReadBoolean("reports", "DebugNoExecution", false);

		CSVReports.MinuteByMinute = config->ReadBoolean("reports", "ReportMinuteByMinute", false);
		CSVReports.CompleteVisitorData = config->ReadBoolean("reports", "ReportCompleteVisitorData", false);
		CSVReports.VisitorLocation = config->ReadBoolean("reports", "ReportVisitorLocation", false);

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
		else if (fm >= Constants::FastPassModeNone && fm <= Constants::FastPassModePlusNoStayBenefit)
		{
			std::cerr << "Invalid FastPassMode (must be between " << Constants::FastPassModeNone << " and " << Constants::FastPassModePlusNoStayBenefit << ")." << std::endl;

			isError = true;
		}
		else
		{
			VisitorCount = vc;
			RideTemplate = rt;
			FastPassMode = fm;

			ParkOpenTime = po;
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