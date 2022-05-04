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


#include <fstream>
#include <iostream>
#include "ReportController.h"
#include "ReportHTML.h"
#include "RideController.h"
#include "Utility.h"
#include "VisitorController.h"


extern RideController* GRideController;
extern VisitorController* GVisitorController;


ReportController* GReportController;


ReportController::ReportController(bool show_output)
{
	ShowOutput = show_output;
}


void ReportController::OutputStatus(std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


void ReportController::SaveRideDataCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Ride data...");
		
		for (int r = 0; r < GRideController->Rides.size(); r++)
		{
			file << GRideController->Rides[r].RideOperation.name << "," << GRideController->Rides[r].DailyStatistics.totalRiders << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Ride data." << std::endl;
	}
}


void ReportController::SaveVisitorDataCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Visitor data...");

   		// header
		file << "Name,Type,Distance Travelled,Distance Travelled per ride,Time spent queuing,Time spent travelling,Time riding,Ride count,Queuing+Travelling/Riding Time,No ride available,Wait time too long,Shutdown,Shortest queue,longest queue,Fastpass rides" << "\n";

		for (int v = 0; v < GVisitorController->Visitors.size(); v++)
		{
			Visitor vx = GVisitorController->Visitors[v];

			float dtbrc = 0;
			float tqttbtr = 0;

			if (vx.Rides.count != 0)
			{
				dtbrc = (float)vx.Rides.distanceTravelled / (float)vx.Rides.count;
			}

			if (vx.TimeSpent.riding != 0)
			{
				tqttbtr = (float)(vx.TimeSpent.queuing + (float)vx.TimeSpent.travelling) / (float)vx.TimeSpent.riding;
			}

			file << vx.Name << "," << vx.Type << "," << vx.Rides.distanceTravelled << "," << dtbrc << "," << vx.TimeSpent.queuing << "," << vx.TimeSpent.travelling << "," << vx.TimeSpent.riding << "," << vx.Rides.count << "," << tqttbtr << "," << vx.Rides.noRideAvailable << "," << vx.Rides.waitTimeTooLong << "," << vx.Rides.rideShutdown << "," << vx.Rides.shortestQueue << "," << vx.Rides.longestQueue << "," << vx.Rides.fastPassRides << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Visitor data." << std::endl;
	}
}


// number of rides completed vs number of visitors
void ReportController::SaveRideCountCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Ride count data...");

		for (int v = 0; v < kDailyRideCount; v++)
		{
			file << v << "," << GVisitorController->DailyStats.rideCount[v] << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Ride count data." << std::endl;
	}
}


void ReportController::SaveMinuteByMinuteLogCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Minute-by-Minute report...");

		std::string output = "minute #";

		// == header ==========================================================================================
		for (int r = 0; r < GRideController->Rides.size(); r++)
		{
			output += "," + GRideController->Rides[r].RideOperation.name + " (Queue Size)," + GRideController->Rides[r].RideOperation.name + " (Riders)," + GRideController->Rides[r].RideOperation.name + " (Wait time)";
		}

		file << output << ",In Park,On Way,At Entrance,Idle,Riding,Queing,Queing FastPass,Travelling,Waiting,Exited,Average Rides,Min Rides,Max Rides" << "\n";

		// == body ============================================================================================
		for (int m = 0; m < GRideController->Rides[0].GetMinuteStatsCount(); m++)
		{
			output = std::to_string(m);

			for (int r = 0; r < GRideController->Rides.size(); r++)
			{
				output += "," + GRideController->Rides[r].GetMinuteStatFor(m);
			}

			file << output << "," << GVisitorController->GetMinuteDataFor(m) << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Minute-by-Minute report." << std::endl;
	}
}


void ReportController::SaveSelectionChoiceCacheCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Selection Choice Cache report...");

		std::string output = "";

		for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
		{
			output = std::to_string(t);

			for (int i = 0; i < kSelectionChoiceCacheCount; i++)
			{
				output += "," + std::to_string(GRideController->SelectionChoiceCache[t][i]);
			}

			file << output << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Selection Choice Cache report." << std::endl;
	}
}


// be warned! the output file will be around 280MB per 20000 visitors!
void ReportController::SaveVisitorLocationCSV(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Visitor Location report...");

		std::string output = "";

		for (int m = 0; m < GRideController->GetMinuteCount(); m++)
		{
			for (int v = 0; v < GVisitorController->Visitors.size(); v++)
			{
				output = GVisitorController->Visitors[v].GetLocationByMinute(m);

				file << v << output << "\n";
			}
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Visitor Location report." << std::endl;
	}
}


void ReportController::SaveSimulationReport(std::string filename, int fast_pass_mode)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving Simulation report...");

		GVisitorController->CalculateStatistics();

		// =======================================================================================================================

		file << "        Simulation Report, Created " << Utility::DateTime(0) << "\n";
		file << "\n";
		file << "  Park Name                   : " << GRideController->ParkName << "\n";
		file << "  Number of rides in park     : " << GRideController->Rides.size() << "\n";
		file << "  Theoretical hourly capacity : " << GRideController->TheoreticalHourThroughputTotal << "\n";
		file << "  FastPass mode               : " << Constants::FastPassModeNames[fast_pass_mode] << "\n";
		file << "\n";

		// =======================================================================================================================

		file << "  Visitor count               : " << GVisitorController->Visitors.size() << "\n";
		file << "  Total Rides                 : " << GVisitorController->DailyStats.totalRides << "\n";
		file << "    FastPass Rides            : " << GVisitorController->DailyStats.totalFastPastRides << "\n";
		file << "    Max Rides                 : " << GVisitorController->DailyStats.maxRides << "\n";
		file << "    Min Rides                 : " << GVisitorController->DailyStats.minRides << "\n";
		file << "    Zero rides                : " << GVisitorController->DailyStats.zeroRides << "\n";
		file << "    Average Rides             : " << GVisitorController->DailyStats.averageRides << "\n";
		file << "    Average Queue Time (mins) : " << GVisitorController->DailyStats.averageQueueTime << "\n";
		file << "\n";

		file << "  No ride available           : " << GVisitorController->DailyStats.noRideAvailable << "\n";
		file << "  Ride shutdown               : " << GVisitorController->DailyStats.rideShutdown << "\n";
		file << "  Wait too long               : " << GVisitorController->DailyStats.waitTimeTooLong << "\n";
		file << "\n";

		file << "  Distance Travelled (m)      : " << GVisitorController->DailyStats.distanceTravelled << "\n";
		file << "  Avg. Distance Travelled (m) : " << GVisitorController->DailyStats.averageDistanceTravelled << "\n";
		file << "\n";

		file << "Average time (min)" << "\n";
		file << "  Idle                        : " << GVisitorController->DailyStats.averageIdleTime << "\n";
		file << "  Queing                      : " << GVisitorController->DailyStats.averageQueueTime << "\n";
		file << "  Riding                      : " << GVisitorController->DailyStats.averageRidingTime << "\n";
		file << "  Travelling                  : " << GVisitorController->DailyStats.averageTravellingTime << "\n";
		file << "\n";

		file << "   visitors by type" << "\n";
		file << "\n";

		for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
		{
			file << "    Type " << t << "  (" << GVisitorController->TypeCount[t] << ")" << "\n";
			file << "\n";

			file << "    No ride available           : " << GVisitorController->DailyStatsByType[t].noRideAvailable << "\n";
			file << "    Wait too long               : " << GVisitorController->DailyStatsByType[t].waitTimeTooLong << "\n";
			file << "\n";

			file << "    Distance Travelled (m)      : " << GVisitorController->DailyStatsByType[t].distanceTravelled << "\n";
			file << "    Avg. Distance Travelled (m) : " << GVisitorController->DailyStatsByType[t].averageDistanceTravelled << "\n";
			file << "\n";

			file << "      Selection choice cache (ride vs frequency of ride choice)" << "\n";
			file << "\n";

			int freq[Constants::MaxRideCount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // max of 30 rides ;)

			for (int i = 0; i < kSelectionChoiceCacheCount; i++)
			{
				freq[GRideController->SelectionChoiceCache[t][i]]++;
			}

			for (int r = 0; r < GRideController->Rides.size(); r++)
			{
				file << "        #" << Utility::PadRight(r, 2) << " " << Utility::PadRight(freq[r], 3) << "\n";
			}

			file << "\n";
		}

		// =======================================================================================================================

		file << "** Park Log ********************************************************" << "\n";
		file << "\n";

		for (int i = 0; i < GRideController->Log.size(); i++)
		{
			file << GRideController->Log[i] << "\n";
		}

		file << "\n";
		file << "********************************************************************" << "\n";
		file << "\n";

		// =======================================================================================================================

		file << "** Rides ***********************************************************" << "\n";
		file << "\n";

		for (int i = 0; i < GRideController->Rides.size(); i++)
		{
			float tpc = ((float)GRideController->Rides[i].DailyStatistics.totalRiders / ((float)GRideController->Rides[i].RideThroughput.totalPerHour * (float)GRideController->Rides[i].RideOperation.operationHours)) * 100.0f;

			file << GRideController->Rides[i].RideOperation.name << "   [" << kRideTypeNames[GRideController->Rides[i].RideOperation.rideType] << "]   x: " << GRideController->Rides[i].RideOperation.position.x<< " y: " << GRideController->Rides[i].RideOperation.position.y << "\n";
			file << "    Unique ID           : " << GRideController->Rides[i].GetUniqueReference() << "\n";
			file << "    Open                : " << Utility::FormatTime(GRideController->Rides[i].RideOperation.open) << " until " << Utility::FormatTime(GRideController->Rides[i].RideOperation.close) << "\n";
			file << "    Length              : " << GRideController->Rides[i].RideOperation.rideLength << " minutes, capacity " << GRideController->Rides[i].RideThroughput.totalPerHour << " per hour" << "\n";
			file << "    Riders today        : " << GRideController->Rides[i].DailyStatistics.totalRiders << "\n";
			file << "    Capacity %          : " << tpc << "\n";
			file << "    Max Queue Length    : " << GRideController->Rides[i].DailyStatistics.maxQueueLength << "\n";

			if (GRideController->Rides[i].FastPass.mode != 0)
			{
				file << "    Throughput (min)    : " << GRideController->Rides[i].RideThroughput.perMinuteFastPass << " (" << GRideController->Rides[i].RideThroughput.perMinuteIFastPass << ")" << "\n";
				file << "    Throughput FP (min) : " << GRideController->Rides[i].RideThroughput.perMinute << " (" << GRideController->Rides[i].RideThroughput.perMinuteI << ")" << "\n";
			}
			else
			{
				file << "    Throughput (min)    : " << GRideController->Rides[i].RideThroughput.perMinuteFastPass << " (" << GRideController->Rides[i].RideThroughput.perMinuteIFastPass << ")" << "\n";
			}

			file << "\n";
		}

		file << "********************************************************************" << "\n";

		// =======================================================================================================================

		file.close();
	}
	else
	{
		std::cerr << "Unable to save Simulation report." << std::endl;
	}
}


// ====================================================================================================================== =
// ====================================================================================================================== =


void ReportController::SaveSimulationReportHTML(std::string filename, int fastpass_mode)
{
	if (!ReportHTML::SaveSimulationReport(filename, fastpass_mode))
	{
		std::cerr << "Unable to save HTML Simulation report." << std::endl;
	}
}