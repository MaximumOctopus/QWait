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


#include <fstream>
#include <iostream>
#include <string>

#include "Configuration.h"
#include "Constants.h"
#include "ParkController.h"
#include "QWaitTypes.h"
#include "ReportText.h"
#include "Utility.h"
#include "VisitorController.h"


extern ParkController* GParkController;
extern VisitorController* GVisitorController;

extern Configuration* GConfiguration;


namespace ReportText
{
	void ReportText::SaveSimulationReport(const std::wstring file_name, FastPassType fast_pass_mode)
	{
		std::wofstream file(file_name);

		if (file)
		{
			OutputStatus(L"Saving Simulation report...");

			GVisitorController->CalculateStatistics();

			// =======================================================================================================================

			file << "        Simulation Report, Created " << Utility::DateTime(kDisplayModeConsole) << "\n";
			file << "\n";
			file << "  Park Name                    : " << GParkController->entrance.ParkName << "\n";
			file << "  Number of rides in park      : " << GParkController->Rides.size() << "\n";
			file << "  Theoretical hourly capacity  : " << GParkController->TheoreticalHourThroughputTotal << "\n";
			file << "  FastPass mode                : " << QWaitTypes::FastPassModeToString(fast_pass_mode) << "\n";
			file << "\n";
			file << "  Tickets purchased:";
			file << "    Adult                      : " << GParkController->entrance.Tickets.Adult << "\n";
			file << "    Adult (advanced)           : " << GParkController->entrance.Tickets.AdultAdvance << "\n";
			file << "    Child                      : " << GParkController->entrance.Tickets.Children << "\n";
			file << "    Child (advanced)           : " << GParkController->entrance.Tickets.ChildrenAdvance << "\n";
			file << "    Baby                       : " << GParkController->entrance.Tickets.Babies << "\n";
			file << "    Baby (advanced)            : " << GParkController->entrance.Tickets.BabiesAdvance << "\n";
			file << "\n";
			file << "  Ticket costs:";
			file << "    Adult                      $ " << GParkController->entrance.Prices.Adult << "\n";
			file << "    Adult (advanced)           $ " << GParkController->entrance.Prices.AdultAdvance << "\n";
			file << "    Child                      $ " << GParkController->entrance.Prices.Children << "\n";
			file << "    Child (advanced)           $ " << GParkController->entrance.Prices.ChildrenAdvance << "\n";
			file << "    Baby                       $ " << GParkController->entrance.Prices.Babies << "\n";
			file << "    Baby (advanced)            $ " << GParkController->entrance.Prices.BabiesAdvance << "\n";
			file << "\n\n";
			file << "    Total                      $ " << GVisitorController->DailyStats.totalSpend << "\n";

			// =======================================================================================================================

			file << "  Groups                       : " << GVisitorController->GroupCountCreated << "\n";
			file << "    Visitor count              : " << GVisitorController->VisitorCountCreated << "\n";
			file << "  Total Rides                  : " << GVisitorController->DailyStats.totalRides << "\n";
			file << "    FastPass Rides             : " << GVisitorController->DailyStats.totalFastPastRides << "\n";
			file << "    Max Rides                  : " << GVisitorController->DailyStats.maxRides << "\n";
			file << "    Min Rides                  : " << GVisitorController->DailyStats.minRides << "\n";
			file << "    Zero rides                 : " << GVisitorController->DailyStats.zeroRides << "\n";
			file << "    Average Rides              : " << GVisitorController->DailyStats.averageRides << "\n";
			file << "    Average Queue Time (mins)  : " << GVisitorController->DailyStats.averageQueueTime << "\n";
			file << "\n";

			file << "  No ride available            : " << GVisitorController->DailyStats.noRideAvailable << "\n";
			file << "  Ride shutdown                : " << GVisitorController->DailyStats.rideShutdown << "\n";
			file << "  Wait too long                : " << GVisitorController->DailyStats.waitTimeTooLong << "\n";
			file << "\n";

			file << "  Distance Travelled (m)       : " << GVisitorController->DailyStats.distanceTravelled << "\n";
			file << "  Avg. Distance Travelled (m)  : " << GVisitorController->DailyStats.averageDistanceTravelled << "\n";
			file << "\n";

			file << "Average time (min)" << "\n";
			file << "  Idle                         : " << GVisitorController->DailyStats.averageIdleTime << "\n";
			file << "  Queing                       : " << GVisitorController->DailyStats.averageQueueTime << "\n";
			file << "  Riding                       : " << GVisitorController->DailyStats.averageRidingTime << "\n";
			file << "  Travelling                   : " << GVisitorController->DailyStats.averageTravellingTime << "\n";
			file << "  Waiting                      : " << GVisitorController->DailyStats.averageWaitingTime << "\n";
			file << "\n";

			file << "Total time spent (min)" << "\n";
			file << "  Idle                         : " << GVisitorController->DailyStats.totalIdleTime << "\n";
			file << "  Queing                       : " << GVisitorController->DailyStats.totalQueueTime << "\n";
			file << "  Riding                       : " << GVisitorController->DailyStats.totalRidingTime << "\n";
			file << "  Travelling                   : " << GVisitorController->DailyStats.totalTravellingTime << "\n";
			file << "  Waiting                      : " << GVisitorController->DailyStats.totalWaitingTime << "\n";
			file << "\n";

			file << "  Queue time per ride time (m) : " << GVisitorController->DailyStats.queueTimePerRideTime << "\n";
			file << "\n";

			// =======================================================================================================================

			file << "===============================================================================\n";
			file << "  Groups by Type" << "\n";
			file << "===============================================================================\n";
			file << "\n";

			for (int t = 0; t < Constants::AvailableGroupTypes; t++)
			{
				file << "  Type \"" << Constants::GroupNames[t] << "\"  (" << GVisitorController->GroupTypeCount[t] << ")" << "\n";
				file << "\n";

				file << "    No ride available              : " << GVisitorController->DailyStatsByGroup[t].noRideAvailable << "\n";
				file << "    Wait too long                  : " << GVisitorController->DailyStatsByGroup[t].waitTimeTooLong << "\n";
				file << "\n";

				file << "    Distance Travelled (m)         : " << GVisitorController->DailyStatsByGroup[t].distanceTravelled << "\n";
				file << "    Avg. Distance Travelled (m)    : " << GVisitorController->DailyStatsByGroup[t].averageDistanceTravelled << "\n";
				file << "\n";

				file << "    Average time (min)" << "\n";
				file << "      Idle                         : " << GVisitorController->DailyStatsByGroup[t].averageIdleTime << "\n";
				file << "      Queing                       : " << GVisitorController->DailyStatsByGroup[t].averageQueueTime << "\n";
				file << "      Riding                       : " << GVisitorController->DailyStatsByGroup[t].averageRidingTime << "\n";
				file << "      Travelling                   : " << GVisitorController->DailyStatsByGroup[t].averageTravellingTime << "\n";
				file << "      Waiting                      : " << GVisitorController->DailyStatsByGroup[t].averageWaitingTime << "\n";
				file << "\n";

				file << "    Total time spent (min)" << "\n";
				file << "      Idle                         : " << GVisitorController->DailyStatsByGroup[t].totalIdleTime << "\n";
				file << "      Queing                       : " << GVisitorController->DailyStatsByGroup[t].totalQueueTime << "\n";
				file << "      Riding                       : " << GVisitorController->DailyStatsByGroup[t].totalRidingTime << "\n";
				file << "      Travelling                   : " << GVisitorController->DailyStatsByGroup[t].totalTravellingTime << "\n";
				file << "      Waiting                      : " << GVisitorController->DailyStatsByGroup[t].totalWaitingTime << "\n";
				file << "\n";

				file << "    Queue time per ride time (m)   : " << GVisitorController->DailyStatsByGroup[t].queueTimePerRideTime << "\n";
				file << "\n";

				//file << "      Selection choice cache (ride vs frequency of ride choice)" << "\n";
				//file << "\n";
			}

			// =======================================================================================================================

			file << "===============================================================================\n";
			file << "  Visitors by Type" << "\n";
			file << "===============================================================================\n";
			file << "\n";

			for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
			{
				file << "  Type \"" << QWaitTypes::VisitorTypeToString(t) << "\"  (" << GVisitorController->TypeCount[t] << ")" << "\n";
				file << "\n";

				file << "    No ride available              : " << GVisitorController->DailyStatsByType[t].noRideAvailable << "\n";
				file << "    Wait too long                  : " << GVisitorController->DailyStatsByType[t].waitTimeTooLong << "\n";
				file << "\n";

				file << "    Distance Travelled (m)         : " << GVisitorController->DailyStatsByType[t].distanceTravelled << "\n";
				file << "    Avg. Distance Travelled (m)    : " << GVisitorController->DailyStatsByType[t].averageDistanceTravelled << "\n";
				file << "\n";

				file << "    Average time (min)" << "\n";
				file << "      Idle                         : " << GVisitorController->DailyStatsByType[t].averageIdleTime << "\n";
				file << "      Queing                       : " << GVisitorController->DailyStatsByType[t].averageQueueTime << "\n";
				file << "      Riding                       : " << GVisitorController->DailyStatsByType[t].averageRidingTime << "\n";
				file << "      Travelling                   : " << GVisitorController->DailyStatsByType[t].averageTravellingTime << "\n";
				file << "      Waiting                      : " << GVisitorController->DailyStatsByType[t].averageWaitingTime << "\n";
				file << "\n";

				file << "    Total time spent (min)" << "\n";
				file << "      Idle                         : " << GVisitorController->DailyStatsByType[t].totalIdleTime << "\n";
				file << "      Queing                       : " << GVisitorController->DailyStatsByType[t].totalQueueTime << "\n";
				file << "      Riding                       : " << GVisitorController->DailyStatsByType[t].totalRidingTime << "\n";
				file << "      Travelling                   : " << GVisitorController->DailyStatsByType[t].totalTravellingTime << "\n";
				file << "      Waiting                      : " << GVisitorController->DailyStatsByType[t].totalWaitingTime << "\n";
				file << "\n";

				file << "    Queue time per ride time (m)   : " << GVisitorController->DailyStatsByType[t].queueTimePerRideTime << "\n";
				file << "\n";

				file << "    Selection choice cache (ride vs frequency of ride choice)" << "\n";
				file << "\n";

				int freq[Constants::MaxRideCount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // max of 30 rides ;)

				for (int i = 0; i < kSelectionChoiceCacheCount; i++)
				{
					freq[GParkController->SelectionChoiceCache[t][i]]++;
				}

				for (int r = 0; r < GParkController->Rides.size(); r++)
				{
					file << "        #" << Utility::PadRight(r, 2) << " " << Utility::PadRight(freq[r], 3) << "\n";
				}

				file << "\n";
			}

			// =======================================================================================================================

			file << "===============================================================================\n";
			file << "  Park Log" << "\n";
			file << "===============================================================================\n";

			for (int i = 0; i < GParkController->Log.size(); i++)
			{
				file << GParkController->Log[i] << "\n";
			}

			file << "\n";
			file << L"===============================================================================\n";
			file << "\n";

			// =======================================================================================================================

			file << L"===============================================================================\n";
			file << L"  Rides" << "\n";
			file << L"===============================================================================\n";
			file << "\n";

			for (int i = 0; i < GParkController->Rides.size(); i++)
			{
				double tpc = ((double)GParkController->Rides[i].DailyStatistics.totalRiders / ((double)GParkController->Rides[i].RideThroughput.totalPerHour * (double)GParkController->Rides[i].RideOperation.operationHours)) * 100.0f;

				file << GParkController->Rides[i].RideOperation.name << L"   [" << kRideTypeNames[GParkController->Rides[i].RideOperation.rideTypeAsInt()] << "]   x: " << GParkController->Rides[i].RideOperation.position.x << " y: " << GParkController->Rides[i].RideOperation.position.y << "\n";
				file << L"    Unique ID           : " << GParkController->Rides[i].GetUniqueReference() << "\n";
				file << L"    Open                : " << Utility::FormatTime(GParkController->Rides[i].RideOperation.open) << L" until " << Utility::FormatTime(GParkController->Rides[i].RideOperation.close) << "\n";
				file << L"    Length              : " << GParkController->Rides[i].RideOperation.rideLength << L" minutes, capacity " << GParkController->Rides[i].RideThroughput.totalPerHour << " per hour" << "\n";
				file << L"    Ride Exit Type      : " << QWaitTypes::RideExitTypeToString(GParkController->Rides[i].RideOperation.rideExitType);
				file << L"    Riders today        : " << GParkController->Rides[i].DailyStatistics.totalRiders << "\n";
				file << L"    Capacity %          : " << tpc << "\n";
				file << L"    Max Queue Length    : " << GParkController->Rides[i].DailyStatistics.maxQueueLength << "\n";

				if (GParkController->Rides[i].FastPass.mode != 0)
				{
					file << L"    Throughput (min)    : " << GParkController->Rides[i].RideThroughput.perMinuteFastPass << " (" << GParkController->Rides[i].RideThroughput.perMinuteIFastPass << ")" << "\n";
					file << L"    Throughput FP (min) : " << GParkController->Rides[i].RideThroughput.perMinute << " (" << GParkController->Rides[i].RideThroughput.perMinuteI << ")" << "\n";
				}
				else
				{
					file << L"    Throughput (min)    : " << GParkController->Rides[i].RideThroughput.perMinuteFastPass << " (" << GParkController->Rides[i].RideThroughput.perMinuteIFastPass << ")" << "\n";
				}

				file << "\n";

				file << L"    Adult valid         : " << GParkController->Rides[i].RideOperation.AdultValid << "\n";
				file << L"    Child valid         : " << GParkController->Rides[i].RideOperation.ChildValid << "\n";
				file << L"    Baby valid          : " << GParkController->Rides[i].RideOperation.BabyValid << "\n";

				file << "\n";
			}

			file << "===============================================================================\n";

			// =======================================================================================================================

			file.close();
		}
		else
		{
			std::wcerr << L"Unable to save Simulation report." << std::endl;
		}
	}


	void ReportText::SaveMxMReport(const std::wstring file_name, FastPassType fast_pass_mode)
	{
		std::wofstream file(file_name);

		if (file)
		{
			OutputStatus(L"Saving Text MxM report...");
				
			for (int m = 0; m < GParkController->Rides[0].GetMinuteStatsCount(); m++)
			{
				MinuteDataV mdv = GVisitorController->GetMinuteDataStructFor(m);

				if (GConfiguration->FastPassMode == FastPassType::None)
				{
					MxMRow(file, mdv);
				}
				else
				{
					MxMRowFP(file, mdv);
				}
			}

			file.close();
		}
		else
		{
			std::wcerr << L"Unable to save Text MxM report." << std::endl;
		}
	}


	void ReportText::MxMRow(std::wofstream& file, MinuteDataV mdv)
	{
		std::wstring pIdle    = L" n/a";
		std::wstring pTravel  = L" n/a";
		std::wstring pQueuing = L" n/a";
		std::wstring pRiding  = L" n/a";
		std::wstring pBusy    = L" n/a";

		if (mdv.visitorsInPark != 0)
		{
			pIdle    = Utility::PadRight(static_cast<int>((double)(mdv.idle) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pTravel  = Utility::PadRight(static_cast<int>((double)(mdv.travelling) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pQueuing = Utility::PadRight(static_cast<int>((double)(mdv.queuing) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pRiding  = Utility::PadRight(static_cast<int>((double)(mdv.riding) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pBusy    = Utility::PadRight(static_cast<int>((double)(mdv.waiting) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
		}

		file << Utility::PadRight(mdv.time, 6) << L"  In park : " << Utility::PadRight(mdv.visitorsInPark, 6) << L"  Idle : " << Utility::PadRight(mdv.idle, 6) << L" (" << pIdle << L")  Travelling : " << Utility::PadRight(mdv.travelling, 6) << L" (" << pTravel << L")  Queuing : " << Utility::PadRight(mdv.queuing, 6) << L" (" << pQueuing << L")  Riding : " << Utility::PadRight(mdv.travelling, 6) << L" (" << pRiding << L")    Busy : " << Utility::PadRight(mdv.waiting, 6) << L" (" << pIdle << L")  Average Rides : " << Utility::PadRight(mdv.averageRides, 3) << "\n";
	}


	void ReportText::MxMRowFP(std::wofstream& file, MinuteDataV mdv)
	{
		std::wstring pIdle      = L" n/a";
		std::wstring pTravel    = L" n/a";
		std::wstring pQueuing   = L" n/a";
		std::wstring pQueuingFP = L" n/a";
		std::wstring pRiding    = L" n/a";
		std::wstring pBusy      = L" n/a";

		if (mdv.visitorsInPark != 0)
		{
			pIdle      = Utility::PadRight(static_cast<int>((double)(mdv.idle) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pTravel    = Utility::PadRight(static_cast<int>((double)(mdv.travelling) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pQueuing   = Utility::PadRight(static_cast<int>((double)(mdv.queuing) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pQueuingFP = Utility::PadRight(static_cast<int>((double)(mdv.queuingFastPass) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pRiding    = Utility::PadRight(static_cast<int>((double)(mdv.riding) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
			pBusy      = Utility::PadRight(static_cast<int>((double)(mdv.waiting) / (double)(mdv.visitorsInPark)) * 100.0f, 3) + L"%";
		}

		file << Utility::PadRight(mdv.time, 6) << L"  In park : " << Utility::PadRight(mdv.visitorsInPark, 6) << L"  Idle : " << Utility::PadRight(mdv.idle, 6) << L" (" << pIdle << L")  Travelling : " << Utility::PadRight(mdv.travelling, 6) << L" (" << pTravel << L")  Queuing : " << Utility::PadRight(mdv.queuing, 6) << L" (" << pQueuing << L")  Queuing : " << Utility::PadRight(mdv.queuingFastPass, 6) << L" (" << pQueuingFP << L")  Riding : " << Utility::PadRight(mdv.travelling, 6) << L" (" << pRiding << L")    Busy : " << Utility::PadRight(mdv.waiting, 6) << L" (" << pIdle << L")  Average Rides : " << Utility::PadRight(mdv.averageRides, 3) << "\n";
	}


	void ReportText::OutputStatus(const std::wstring status)
	{
		if (!GConfiguration->DebugConfig.DisableConsoleOutput)
		{
			std::wcout << status << std::endl;
		}
	}
}