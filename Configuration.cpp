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


Configuration::Configuration(int argc, wchar_t* argv[])
{
	if (argc != 0)
	{
		for (int t = 1; t < argc; t++)
		{
			std::wstring parameter = argv[t];

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
				std::wcerr << L"Error, invalid parameter \"" << parameter << "\"." << std::endl;
			}
		}
	}

	SetDefaults();

	SetFromCommandLine();
}


void Configuration::SetDefaults()
{
	RideTemplate = ParkTemplate::WDWAnimalKingdom;
	RideTemplateFile = L"";
	CSVReports.VisitorDemographicsFile = L"";

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
	HTMLReports.SimulationReportFile = L"";

	TextReports.SimulationReport = false;

	Input.VisitorDemographics = false;
	Input.VisitorDemographicsFile = L"";

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
			if (Parameters[p].command.find(kDebugDistanceCache) != std::wstring::npos)
			{
				DebugReports.DistanceCache = true;
			}
			else if (Parameters[p].command.find(kDebugSCC) != std::wstring::npos)
			{
				DebugReports.SelectionChoiceCache = true;
			}
			else if (Parameters[p].command.find(kDegbugNoExecute) != std::wstring::npos)
			{
				DebugConfig.NoExecution = true;
			}
			else if (Parameters[p].command.find(kReportCVD) != std::wstring::npos)
			{
				CSVReports.CompleteVisitorData = true;
			}
			else if (Parameters[p].command.find(kReportMxMCSV) != std::wstring::npos)
			{
				CSVReports.MinuteByMinute = true;
			}
			else if (Parameters[p].command.find(kReportMxMText) != std::wstring::npos)
			{
				TextReports.MinuteByMinute = true;
			}
			else if (Parameters[p].command.find(kReportSimulation) != std::wstring::npos)
			{
				TextReports.SimulationReport = true;
			}
			else if (Parameters[p].command.find(kReportVisitorLocation) != std::wstring::npos)
			{
				// be warned! the output file will be around 280MB per 20000 visitors!
				CSVReports.VisitorLocation = true;
			}
			else if (Parameters[p].command.find(kReportVisitorRideList) != std::wstring::npos)
			{
				CSVReports.VisitorRideList = true;
			}
			else if (Parameters[p].command.find(kGetVisitorNames) != std::wstring::npos)
			{
				GetVisitorNames = true;
			}
			else if (Parameters[p].command.find(kDebugUpdateRate) != std::wstring::npos)
			{
				HandleDebugUpdateRate(stoi(Parameters[p].property));
			}
			else if (Parameters[p].command.find(kImportVisitorDemo) != std::wstring::npos)
			{
				HandleImportVisitorDemo(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kReportSimulationHTML) != std::wstring::npos)
			{
				HandleSimulationReportHTML(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kReportVisitorDemo) != std::wstring::npos)
			{
				HandleReportVisitorDemo(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kTemplateFile) != std::wstring::npos)
			{
				HandleTemplateFile(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kTemplate) != std::wstring::npos)
			{
				HandleTemplate(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kVisitors) != std::wstring::npos)
			{
				HandleVisitors(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kFastPassMode) != std::wstring::npos)
			{
				HandleFastPassMode(Parameters[p].property);
			}
			else if (Parameters[p].command.find(kUserConfigFile) != std::wstring::npos)
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
		std::wcerr << L"Error, invalid update rate \"" << update_rate << "\". Using 15 (default)." << std::endl;
	}
}


void Configuration::HandleImportVisitorDemo(const std::wstring file_name)
{
	if (file_name != L"")
	{
		Input.VisitorDemographicsFile = SetExtension(file_name, L".csv");

		Input.VisitorDemographics = true;
	}
	else
	{
		std::wcerr << L"Error, no visitor demo ouput file specified. Using random visitor demographics." << std::endl;
	}
}


void Configuration::HandleSimulationReportHTML(const std::wstring file_name)
{
	if (file_name != L"")
	{
		HTMLReports.SimulationReportFile = SetExtension(file_name, L".html");
	}
	else
	{
		HTMLReports.SimulationReportFile = Utility::DateTime(kDisplayModeFile) + L".html";

		std::wcerr << L"No html report ouput file specified, using \"" << HTMLReports.SimulationReportFile << "\"." << std::endl;
	}

	HTMLReports.SimulationReport = true;
}


void Configuration::HandleReportVisitorDemo(const std::wstring file_name)
{
	if (file_name != L"")
	{
		CSVReports.VisitorDemographicsFile = SetExtension(file_name, L".csv");

		CSVReports.VisitorDemographics = true;
	}
	else
	{
		std::wcerr << L"Error, no visitor demo ouput file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplateFile(const std::wstring file_name)
{
	if (file_name != L"")
	{
		RideTemplateFile = SetExtension(file_name, L".template");
	}
	else
	{
		std::wcerr << L"Error, no template file specified. Using template 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleTemplate(const std::wstring property)
{
	if (property != L"")
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
			std::wcerr << L"Invalid template ID parameter: " << property << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Error, no template specified. Using 1 (Animal Kingdom)." << std::endl;
	}
}


void Configuration::HandleVisitors(const std::wstring property)
{
	if (property != L"")
	{
		try
		{
			VisitorCount = std::stoi(property);
		}
		catch (...)
		{
			std::wcout << L"Invalid visitor count parameter: " << property << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Error, no visitor count specified. Using 10000." << std::endl;
	}
}


void Configuration::HandleFastPassMode(const std::wstring property)
{
	if (property != L"")
	{
		try
		{
			int fpm = std::stoi(property);

			FastPassMode = QWaitTypes::IntToFastPassMode(fpm);
		}
		catch (...)
		{
			std::wcout << L"Invalid FastPass mode parameter: " << property << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Error, no FastPass mode specified. Using 0 (none)." << std::endl;
	}
}


void Configuration::HandleUseConfigFile(std::wstring file_name)
{
	if (file_name == L"")
	{
		file_name = L"QWaitConfig.ini";
	}
	else
	{
		file_name = SetExtension(file_name, L".ini");
	}

	if (!LoadConfigurationFromIni(file_name))
	{
		std::wcerr << L"Error, couldn't load config \"" + file_name + L"\". Ignored." << std::endl;
	}
}


std::wstring Configuration::GetCommandValue(const std::wstring input)
{
	size_t pos = input.find(';');

	if (pos != std::wstring::npos)
	{
		return input.substr(0, pos);
	}

	return input;
}


std::wstring Configuration::GetPropertyValue(const std::wstring input)
{
	size_t pos = input.find(';');

	if (pos != std::wstring::npos)
	{
		return input.substr(pos + 1);
	}

	return L"";
}


bool Configuration::IsValidParameter(const std::wstring input)
{
	std::wstring parameter = input;

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
	else if (parameter.find(kDebugUpdateRate) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kImportVisitorDemo) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kReportSimulationHTML) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kReportVisitorDemo) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kReportVisitorRideList) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kTemplateFile) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kTemplate) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kVisitors) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kFastPassMode) != std::wstring::npos)
	{
		return true;
	}
	else if (parameter.find(kUserConfigFile) != std::wstring::npos)
	{
		return true;
	}

	return false;
}


std::wstring Configuration::SetExtension(const std::wstring file_name, const std::wstring extension)
{
	if (file_name.find(extension) == std::wstring::npos)
	{
		return file_name + extension;
	}

	return file_name;
}


bool Configuration::LoadConfigurationFromIni(const std::wstring file_name)
{
	Ini* config = new Ini(file_name);

	bool isError = false;

	if (config->Loaded)
	{
		// == operation ==========================================================================================

		int vc = config->ReadInteger(L"main", L"VisitorCount", 0);
		int rt = config->ReadInteger(L"main", L"RideTemplate", -1);
		int fm = config->ReadInteger(L"main", L"FastPassMode", 0);

		int po = config->ReadInteger(L"main", L"ParkOpenTime", 9);
		int pc = config->ReadInteger(L"main", L"ParkCloseTime", 22);

		// == reports ============================================================================================

		DebugReports.DistanceCache = config->ReadBoolean(L"reports", L"DebugDistanceCache", false);
		DebugReports.SelectionChoiceCache = config->ReadBoolean(L"reports", L"DebugSelectionChoiceCache", false);
		DebugConfig.UpdateRate = config->ReadInteger(L"reports", L"DebugUpdateRate", 15);
		DebugConfig.NoExecution = config->ReadBoolean(L"reports", L"DebugNoExecution", false);

		CSVReports.MinuteByMinute = config->ReadBoolean(L"reports", L"ReportMinuteByMinute", false);
		CSVReports.CompleteVisitorData = config->ReadBoolean(L"reports", L"ReportCompleteVisitorData", false);
		CSVReports.VisitorLocation = config->ReadBoolean(L"reports", L"ReportVisitorLocation", false);
		CSVReports.VisitorRideList = config->ReadBoolean(L"reports", L"ReportVisitorRideList", false);

		HTMLReports.SimulationReport = config->ReadBoolean(L"reports", L"ReportHTML", false);

		TextReports.MinuteByMinute = config->ReadBoolean(L"reports", L"ReportTextMXM", false);
		TextReports.SimulationReport = config->ReadBoolean(L"reports", L"SimulationReport", false);

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