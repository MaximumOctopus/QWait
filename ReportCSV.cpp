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

#include "Configuration.h"
#include "ParkController.h"
#include "ReportCSV.h"
#include "VisitorController.h"


extern ParkController* GParkController;
extern VisitorController* GVisitorController;

extern Configuration* GConfiguration;


namespace ReportCSV
{
	void ReportCSV::SaveRideData(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Ride data...");

			for (int r = 0; r < GParkController->Rides.size(); r++)
			{
				file << GParkController->Rides[r].RideOperation.name << "," << GParkController->Rides[r].DailyStatistics.totalRiders << "\n";
			}

			file.close();
		}
		else
		{
			std::cerr << "Unable to save Ride data." << std::endl;
		}
	}


	void ReportCSV::SaveVisitorData(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Visitor data...");

			// header
			file << "Group,Group Type,Name,Spent,Type,Distance Travelled,Distance Travelled per ride,Time spent queuing,Time spent travelling,Time riding,Ride count,Queuing+Travelling/Riding Time,No ride available,Wait time too long,Shutdown,Shortest queue,longest queue,Fastpass rides" << "\n";

			for (int g = 0; g < GVisitorController->Groups.size(); g++)
			{
				for (int v = 0; v < GVisitorController->Groups[g].Visitors.size(); v++)
				{
					Visitor vx = GVisitorController->Groups[g].Visitors[v];

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

					file << g << "," << GVisitorController->Groups[g].Configuration.GetTypeToInt() << "," << vx.Configuration.Name << "," << vx.Configuration.MoneySpent << "," << vx.Configuration.TypeInt << "," << vx.Rides.distanceTravelled << "," << dtbrc << "," << vx.TimeSpent.queuing << "," << vx.TimeSpent.travelling << "," << vx.TimeSpent.riding << "," << vx.Rides.count << "," << tqttbtr << "," << vx.Rides.noRideAvailable << "," << vx.Rides.waitTimeTooLong << "," << vx.Rides.rideShutdown << "," << vx.Rides.shortestQueue << "," << vx.Rides.longestQueue << "," << vx.Rides.fastPassRides << "\n";
				}
			}

			file.close();
		}
		else
		{
			std::cerr << "Unable to save Visitor data." << std::endl;
		}
	}


	// number of rides completed vs number of visitors
	void ReportCSV::SaveRideCount(const std::string file_name)
	{
		std::ofstream file(file_name);

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


	void ReportCSV::SaveMinuteByMinuteLog(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Minute-by-Minute report...");

			std::string output = "minute #";

			// == header ==========================================================================================
			for (int r = 0; r < GParkController->Rides.size(); r++)
			{
				output += "," + GParkController->Rides[r].RideOperation.name + " (Queue Size)," + GParkController->Rides[r].RideOperation.name + " (Riders)," + GParkController->Rides[r].RideOperation.name + " (Wait time)";
			}

			file << output << ",In Park,On Way,At Entrance,Idle,Riding,Queing,Queing FastPass,Travelling,Waiting,Exited,Average Rides,Min Rides,Max Rides" << "\n";

			// == body ============================================================================================
			for (int m = 0; m < GParkController->Rides[0].GetMinuteStatsCount(); m++)
			{
				output = std::to_string(m);

				for (int r = 0; r < GParkController->Rides.size(); r++)
				{
					output += "," + GParkController->Rides[r].GetMinuteStatFor(m);
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


	void ReportCSV::SaveSelectionChoiceCache(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Selection Choice Cache report...");

			std::string output = "";

			for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
			{
				output = std::to_string(t);

				for (int i = 0; i < kSelectionChoiceCacheCount; i++)
				{
					output += "," + std::to_string(GParkController->SelectionChoiceCache[t][i]);
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
	void ReportCSV::SaveVisitorLocation(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Visitor Location report...");

			std::string output = "";

			for (int m = 0; m < GParkController->GetMinuteCount(); m++)
			{
				for (int g = 0; g < GVisitorController->Groups.size(); g++)
				{
					output = GVisitorController->Groups[g].GetLocationByMinute(m);

					file << g << output << "\n";
				}
			}

			file.close();
		}
		else
		{
			std::cerr << "Unable to save Visitor Location report." << std::endl;
		}
	}


	// saves a complete list of visitors and each ride ridden
	void ReportCSV::SaveVisitorRideList(const std::string file_name)
	{
		std::ofstream file(file_name);

		if (file)
		{
			OutputStatus("Saving Visitor Ride List report...");

			std::string output = "";

			file << "Group ID,Visitor ID,Ride Count,Ride List" << "\n";

			for (int g = 0; g < GVisitorController->Groups.size(); g++)
			{
				for (int v = 0; v < GVisitorController->Groups[g].Visitors.size(); v++)
				{
					output = "," + std::to_string(GVisitorController->Groups[g].Visitors[v].RideList.size());

					if (GVisitorController->Groups[g].Visitors[v].RideList.size() != 0)
					{
						for (int r = 0; r < GVisitorController->Groups[g].Visitors[v].RideList.size(); r++)
						{
							output += "," + GParkController->Rides[GVisitorController->Groups[g].Visitors[v].RideList[r]].RideOperation.name;
						}
					}

					file << g << "," << v << output << "\n";
				}
			}

			file.close();
		}
		else
		{
			std::cerr << "Unable to save Visitor Ride List." << std::endl;
		}
	}


	void ReportCSV::OutputStatus(const std::string status)
	{
		if (!GConfiguration->DebugConfig.DisableConsoleOutput)
		{
			std::cout << status << std::endl;
		}
	}
}