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


#include "Configuration.h"
#include "GlobalController.h"
#include "ParkController.h"
#include "ReportController.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern ReportController* GReportController;
extern ParkController* GParkController;
extern VisitorController* GVisitorController;


namespace GlobalControllers
{
	int CreateGlobalControllers(int argc, char* argv[])
	{
		GConfiguration     = new Configuration(argc, argv);

		GParkController = new ParkController(!GConfiguration->DebugConfig.DisableConsoleOutput, GConfiguration->RideTemplateFile);

		if (GConfiguration->VisitorCount == VisitorCountUseParkAverage)
		{
			GConfiguration->VisitorCount = GParkController->entrance.averageVisitors;
		}

		GVisitorController = new VisitorController(GConfiguration->VisitorCount, !GConfiguration->DebugConfig.DisableConsoleOutput, GConfiguration->RideTemplate, GConfiguration->GetVisitorNames);
		
		return 0;
	}

	void FreeGlobalControllers()
	{
		if (GParkController != nullptr)
		{
			delete GParkController;
		}

		if (GVisitorController != nullptr)
		{
			delete GVisitorController;
		}

		if (GConfiguration != nullptr)
		{
			delete GConfiguration;
		}
	}
};
