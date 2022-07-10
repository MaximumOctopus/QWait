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


int DoWeJustNeedToShowHelp(int argc, char* argv[])
{
    if (argc == 1)
    {
        return 1;
    }

    if (argc >= 2)
    {
        std::string Parameter = argv[1];

        if (Parameter.find("/?") != std::string::npos)
        {
            return 1;
        }

        if (Parameter.find(kCats) != std::string::npos)
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
        GReportController->SaveSelectionChoiceCacheCSV(Utility::DateTime(kDisplayModeFile) + "_sscc.csv");
    }

    if (GConfiguration->DebugReports.DistanceCache)
    {
        GParkController->SaveDistanceCache(Utility::DateTime(kDisplayModeFile) + "_dctc.csv");
    }

    // ==========================================================================================

    if (GConfiguration->CSVReports.CompleteVisitorData)
    {
        GReportController->SaveVisitorDataCSV(Utility::DateTime(kDisplayModeFile) + "_visitors.csv");
    }

    if (GConfiguration->CSVReports.MinuteByMinute)
    {
        GReportController->SaveMinuteByMinuteLogCSV(Utility::DateTime(kDisplayModeFile) + "_mxm.csv");
    }

    if (GConfiguration->CSVReports.VisitorLocation)
    {
        GReportController->SaveVisitorLocationCSV(Utility::DateTime(kDisplayModeFile) + "_location.csv");
    }

    if (GConfiguration->CSVReports.VisitorRideList)
    {
        GReportController->SaveVisitorRideListCSV(Utility::DateTime(kDisplayModeFile) + "_ridelist.csv");
    }

    // ==========================================================================================

    if (GConfiguration->HTMLReports.SimulationReport)
    {
        GReportController->SaveSimulationReportHTML(GConfiguration->HTMLReports.SimulationReportFile, GConfiguration->FastPassMode);
    }

    // ==========================================================================================

    if (GConfiguration->TextReports.MinuteByMinute)
    {
        GReportController->SaveMXMReportText(Utility::DateTime(kDisplayModeFile) + "_mxm.txt", GConfiguration->FastPassMode);
    }

    if (GConfiguration->TextReports.SimulationReport)
    {
        GReportController->SaveSimulationReportText(Utility::DateTime(kDisplayModeFile) + "_sim.txt", GConfiguration->FastPassMode);
    }
}


int main(int argc, char* argv[])
{
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
            Engine e(GConfiguration->FastPassMode, GConfiguration->ParkOpenTime, GConfiguration->ParkCloseTime, !GConfiguration->DebugConfig.DisableConsoleOutput, GConfiguration->DebugConfig.UpdateRate);

            e.Run(GConfiguration->CSVReports.MinuteByMinute || GConfiguration->HTMLReports.SimulationReport || GConfiguration->TextReports.MinuteByMinute, GConfiguration->CSVReports.VisitorLocation);
        }
        else
        {
            std::cerr << "No visitors selected so simulation aborted." << std::endl;
        }
    }

    // ==========================================================================================

    GVisitorController->CalculateStatistics();

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
        std::cout << "\n" << "Done." << std::endl;
        std::cin.get();
    }

    GlobalControllers::FreeGlobalControllers();

    return 0;
}