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


#include <iostream>
#include <string.h>
#include "Configuration.h"
#include "Engine.h"
#include "Help.h"
#include "GlobalController.h"
#include "ReportController.h"
#include "RideController.h"
#include "Utility.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern ReportController* GReportController;
extern RideController* GRideController;
extern VisitorController* GVisitorController;


bool DoWeJustNeedToShowHelp(int argc, char* argv[])
{
    if (argc == 1)
    {
        return true;
    }

    if (argc >= 2)
    {
        std::string Parameter = argv[1];

        if (Parameter.find("/?") != std::string::npos)
        {
            return true;
        }
    }

    return false;
}


void CreateOutput()
{
    if (GConfiguration->DebugReports.SelectionChoiceCache)
    {
        GReportController->SaveSelectionChoiceCacheCSV(Utility::DateTime(1) + "_sscc.csv");
    }

    // ==========================================================================================

    if (GConfiguration->CSVReports.CompleteVisitorData)
    {
        GReportController->SaveVisitorDataCSV(Utility::DateTime(1) + "_visitors.csv");
    }

    if (GConfiguration->CSVReports.MinuteByMinute)
    {
        GReportController->SaveMinuteByMinuteLogCSV(Utility::DateTime(1) + "_mxm.csv");
    }

    if (GConfiguration->CSVReports.VisitorLocation)
    {
        GReportController->SaveVisitorLocationCSV(Utility::DateTime(1) + "_location.csv");
    }

    // ==========================================================================================

    if (GConfiguration->HTMLReports.SimulationReport)
    {
        GReportController->SaveSimulationReportHTML(GConfiguration->HTMLReports.SimulationReportFile, GConfiguration->FastPassMode);
    }

    // ==========================================================================================

    if (GConfiguration->TextReports.SimulationReport)
    {
        GReportController->SaveSimulationReport(Utility::DateTime(1) + "_sim.txt", GConfiguration->FastPassMode);
    }
}


int main(int argc, char* argv[])
{
    if (DoWeJustNeedToShowHelp(argc, argv))
    {
        Help::ShowHelp();

        return 0;
    }

    // ==========================================================================================

    GlobalControllers::CreateGlobalControllers(argc, argv);
        
    GVisitorController->Run(GConfiguration->CSVReports.VisitorDemographics, GConfiguration->CSVReports.VisitorDemographicsFile,
                            GConfiguration->Input.VisitorDemographics, GConfiguration->Input.VisitorDemographicsFile);

    GRideController->BuildRides(GConfiguration->RideTemplate, GConfiguration->RideTemplateFile);

    // ==========================================================================================

    if (!GConfiguration->DebugConfig.NoExecution)
    {
        if (GConfiguration->VisitorCount > 0)
        {
            Engine e(GConfiguration->FastPassMode, GConfiguration->ParkOpenTime, GConfiguration->ParkCloseTime, !GConfiguration->DebugConfig.DisableConsoleOutput);

            e.Run(GConfiguration->CSVReports.MinuteByMinute || GConfiguration->HTMLReports.SimulationReport, GConfiguration->CSVReports.VisitorLocation);
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
        GRideController->ShowStatistics(GConfiguration->ParkOpenHours);
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