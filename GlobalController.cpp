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


#include "Configuration.h"
#include "GlobalController.h"
#include "ReportController.h"
#include "RideController.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern ReportController* GReportController;
extern RideController* GRideController;
extern VisitorController* GVisitorController;


namespace GlobalControllers
{
	int CreateGlobalControllers(int argc, char* argv[])
	{
		GConfiguration     = new Configuration(argc, argv);

		GRideController    = new RideController(!GConfiguration->DebugConfig.DisableConsoleOutput, GConfiguration->RideTemplateFile);

		if (GConfiguration->VisitorCount == VisitorCountUseParkAverage)
		{
			GConfiguration->VisitorCount = GRideController->entrance.averageVisitors;
		}

		GVisitorController = new VisitorController(GConfiguration->VisitorCount, !GConfiguration->DebugConfig.DisableConsoleOutput);
		
		return 0;
	}

	void FreeGlobalControllers()
	{
		if (GRideController != nullptr)
		{
			delete GRideController;
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
