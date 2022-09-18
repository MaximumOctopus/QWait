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

#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

#include "Configuration.h"
#include "Engine.h"
#include "Help.h"
#include "GlobalController.h"
#include "ParkController.h"
#include "ReportController.h"
#include "Utility.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern ReportController* GReportController;
extern ParkController* GParkController;
extern VisitorController* GVisitorController;


int DoWeJustNeedToShowHelp(int argc, wchar_t* argv[])
{
    if (argc == 1)
    {
        return 1;
    }

    if (argc >= 2)
    {
        std::wstring Parameter = argv[1];

        if (Parameter.find(L"/?") != std::wstring::npos)
        {
            return 1;
        }

        if (Parameter.find(kCats) != std::wstring::npos)
        {
            return 2;
        }
    }

    return 0;
}


void CreateOutput()
{
    if (GConfiguration->DebugReports.SelectionChoiceCache)
    {
        GReportController->SaveSelectionChoiceCacheCSV(Utility::DateTime(kDisplayModeFile) + L"_sscc.csv");
    }

    if (GConfiguration->DebugReports.DistanceCache)
    {
        GParkController->SaveDistanceCache(Utility::DateTime(kDisplayModeFile) + L"_dctc.csv");
    }

    // ==========================================================================================

    if (GConfiguration->CSVReports.CompleteVisitorData)
    {
        GReportController->SaveVisitorDataCSV(Utility::DateTime(kDisplayModeFile) + L"_visitors.csv");
    }

    if (GConfiguration->CSVReports.MinuteByMinute)
    {
        GReportController->SaveMinuteByMinuteLogCSV(Utility::DateTime(kDisplayModeFile) + L"_mxm.csv");
    }

    if (GConfiguration->CSVReports.VisitorLocation)
    {
        GReportController->SaveVisitorLocationCSV(Utility::DateTime(kDisplayModeFile) + L"_location.csv");
    }

    if (GConfiguration->CSVReports.VisitorRideList)
    {
        GReportController->SaveVisitorRideListCSV(Utility::DateTime(kDisplayModeFile) + L"_ridelist.csv");
    }

    // ==========================================================================================

    if (GConfiguration->HTMLReports.SimulationReport)
    {
        GReportController->SaveSimulationReportHTML(GConfiguration->HTMLReports.SimulationReportFile, GConfiguration->FastPassMode);
    }

    // ==========================================================================================

    if (GConfiguration->TextReports.MinuteByMinute)
    {
        GReportController->SaveMXMReportText(Utility::DateTime(kDisplayModeFile) + L"_mxm.txt", GConfiguration->FastPassMode);
    }

    if (GConfiguration->TextReports.SimulationReport)
    {
        GReportController->SaveSimulationReportText(Utility::DateTime(kDisplayModeFile) + L"_sim.txt", GConfiguration->FastPassMode);
    }
}


int wmain(int argc, wchar_t* argv[])
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    // ==========================================================================================

    int helporcats = DoWeJustNeedToShowHelp(argc, argv);
    
    if (helporcats == 1)
    {
        Help::ShowHelp();

        return 0;
    }
    else if (helporcats == 2)
    {
        Help::ShowCat();

        return 0;
    }

    // ==========================================================================================

    srand((unsigned)time(NULL));

    // ==========================================================================================

    GlobalControllers::CreateGlobalControllers(argc, argv);
        
    GVisitorController->Run(GConfiguration->CSVReports.VisitorDemographics, GConfiguration->CSVReports.VisitorDemographicsFile,
                            GConfiguration->Input.VisitorDemographics, GConfiguration->Input.VisitorDemographicsFile);

    GParkController->BuildRides(GConfiguration->RideTemplate, GConfiguration->RideTemplateFile);

    // ==========================================================================================

    if (!GConfiguration->DebugConfig.NoExecution)
    {
        if (GConfiguration->VisitorCount > 0)
        {
            Engine e(GConfiguration->FastPassMode, 
                GConfiguration->ParkOpenTime, 
                GConfiguration->ParkCloseTime, 
                !GConfiguration->DebugConfig.DisableConsoleOutput, 
                GConfiguration->DebugConfig.UpdateRate,
                GConfiguration->HandleFoodDrink,
                GConfiguration->Climate.Temperature);

            e.Run(GConfiguration->CSVReports.MinuteByMinute || GConfiguration->HTMLReports.SimulationReport || GConfiguration->TextReports.MinuteByMinute, GConfiguration->CSVReports.VisitorLocation);
        }
        else
        {
            std::wcerr << L"No visitors selected so simulation aborted.\n";
        }
    }

    // ==========================================================================================

    GVisitorController->CalculateStatistics(GParkController->TotalEaterySpending());

    if (!GConfiguration->DebugConfig.DisableConsoleOutput)
    {
        GParkController->ShowStatistics(GConfiguration->ParkOpenHours);

        GVisitorController->ShowStatistics();
    }

    // ==========================================================================================
    // ==========================================================================================

    CreateOutput();

    // ==========================================================================================
    // ==========================================================================================

    if (!GConfiguration->DebugConfig.DisableConsoleOutput)
    {
        std::wcout << "\n" << "Done." << std::endl;
        std::cin.get();
    }

    GlobalControllers::FreeGlobalControllers();

    return 0;
}