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
#include <sstream>
#include <time.h>
#include "Constants.h"
#include "Names.h"
#include "Utility.h"
#include "Visitor.h"
#include "VisitorController.h"


VisitorController* GVisitorController;


VisitorController::VisitorController(int count, bool show_output)
{
	VisitorCount = count;

	ShowOutput = show_output;

	Visitors.reserve(count);

	SetDefaults();
}


void VisitorController::SetDefaults()
{
	for (int x = 0; x < kDailyRideCount; x++)
	{
		DailyStats.rideCount[x] = 0;
	}
}


int VisitorController::GetType(float random)
{
	if (random < 0.06f)
	{
		return Constants::VisitorTypeEnthusiast;
	}
	else if (random < 0.11f)
	{
		return Constants::VisitorTypeFan;
	}
	else if (random < 0.40f)
	{
		return Constants::VisitorTypeTourist;
	}
	else if (random < 0.45f)
	{
		return Constants::VisitorTypeActivity;
	}
	else if (random < 0.75f)
	{
		return Constants::VisitorTypePassholder;
	}
	else if (random < 0.98f)
	{
		return Constants::VisitorTypeEPassHolder;
	}

    return Constants::VisitorTypeTikToker;
}


void VisitorController::Run(bool save_visitor_list, std::string save_filename, bool load_visitor_list, std::string load_filename)
{
	if (load_visitor_list)
	{
		OutputStatus("Loading Visitor data from \"" + load_filename + "\"...");
	}
	else
	{
		OutputStatus("Generating " + std::to_string(VisitorCount) + " visitors...");
	}

	if (load_visitor_list)
	{
		int count = LoadVisitorList(load_filename);

		OutputStatus("Loaded " + std::to_string(count) + " visitors.");
	}
	else
	{
		//Names NameController;

		srand((unsigned)time(NULL));

		for (int v = 0; v < VisitorCount; v++)
		{
			int VisitorType = GetType((float)rand() / RAND_MAX);

			TypeCount[VisitorType]++;

			Visitor visitor(GetTypeConfiguration(VisitorType));

			//visitor.Name = NameController.GetRandomName();

			Visitors.push_back(visitor);
		}
	}

	if (save_visitor_list)
	{
		SaveVisitorList(save_filename);
	}

	ShowConfig();
}


bool VisitorController::CreateVisitorsFromFileData(std::string input)
{
	std::stringstream visitorinput(input);
	std::vector<std::string> parameters;
	std::string parameter;

	while (std::getline(visitorinput, parameter, ','))
	{
		parameters.push_back(parameter);
	}

	if (parameters.size() == 10)
	{
		TypeConfiguration typeconfiguration;

		try
		{
			typeconfiguration.name = parameters[0];
			typeconfiguration.Type = stoi(parameters[1]);

			typeconfiguration.maxWaitTime = stof(parameters[2]);
			typeconfiguration.preference = stof(parameters[3]);

			typeconfiguration.stayDuration = stoi(parameters[4]);

			typeconfiguration.arrivalTime.hours = stoi(parameters[5]);
			typeconfiguration.arrivalTime.minutes = stoi(parameters[6]);
			typeconfiguration.departureTime.hours = stoi(parameters[7]);
			typeconfiguration.departureTime.minutes = stoi(parameters[8]);

			typeconfiguration.stayingOnSite = stoi(parameters[9]);

			TypeCount[typeconfiguration.Type]++;

			Visitor visitor(typeconfiguration);

			Visitors.push_back(visitor);

			return true;
		}
		catch (...)
		{
			std::cerr << "There appears to be some dodgy data in this input row:" << "\n";
			std::cerr << "    \"" << input << "\"" << std::endl;
		}
	}
	else
	{
		std::cerr << "Invalid number of parameters in this visitor data (should be 10): " << parameters.size() << "\n";
		std::cerr << "    \"" << input << "\"" << std::endl;
	}

	return false;
}


void VisitorController::ShowConfig()
{
	if (ShowOutput)
	{
		for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
		{
			std::cout << "Type : " << t << "  " << Utility::PadRight(TypeCount[t], 6) << "\n";
		}

		std::cout << std::endl;
	}
}


TypeConfiguration VisitorController::GetTypeConfiguration(int type)
{
	TypeConfiguration vtc;

	vtc.Type = type;

	switch (type)
	{
	case Constants::VisitorTypeEnthusiast:
		vtc.maxWaitTime = 240.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEnthusiast];
		vtc.stayDuration = 11;

		vtc.arrivalTime.hours = 10;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypeFan:
		vtc.maxWaitTime = 120.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeFan];
		vtc.stayDuration = 10;

		vtc.arrivalTime.hours = 10;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypeTourist:
		vtc.maxWaitTime = 90.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeTourist];
		vtc.stayDuration = 11 + rand() % 2;

		vtc.arrivalTime.hours = 10;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypeActivity:
		vtc.maxWaitTime = 70.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeActivity];
		vtc.stayDuration = 8;

		vtc.arrivalTime.hours = 10 + rand() % 4;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypePassholder:
		vtc.maxWaitTime = 60.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypePassholder];
		vtc.stayDuration = 7;

		vtc.arrivalTime.hours = 10 + rand() % 5;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypeEPassHolder:
		vtc.maxWaitTime = 60.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEPassHolder];
		vtc.stayDuration = 6;

		vtc.arrivalTime.hours = 10 + rand() % 6;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;

	case Constants::VisitorTypeTikToker:
		vtc.maxWaitTime = 45.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeTikToker];
		vtc.stayDuration = 7;

		vtc.arrivalTime.hours = 10 + rand() % 4;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration;

		break;
	}

	vtc.arrivalTime.minutes = rand() % 60;

	// a certain percentage of visitors are staying on-site and get in early
	int OnSite = rand() % 100;

	if (OnSite < Constants::PercentageStayingOnSiteStartingEarly)
	{
		vtc.stayingOnSite = true;

		vtc.arrivalTime.hours = 9;
		vtc.departureTime.hours = vtc.arrivalTime.hours + vtc.stayDuration + rand() % 2;

		vtc.arrivalTime.minutes = rand() % 11; // if you're arriving for 9am you probably aren't going to be too late getting there ;)
	}
	else
	{
		vtc.stayingOnSite = false;
	}

	vtc.departureTime.minutes = vtc.arrivalTime.minutes;

	return vtc;
}


int VisitorController::LoadVisitorList(std::string filename)
{
	int count = 0;

	std::ifstream file(filename);

	if (file)
	{
		OutputStatus("Loading Visitors from file...");

		std::string s;

		while (std::getline(file, s))
		{
			if (s != "")
			{
				if (CreateVisitorsFromFileData(s))
				{
					count++;
				}
			}
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to load visitors from file." << std::endl;
	}

	return count;
}


// saves the randomly generated visitors (without any stats or sim information)
// this is useful if you want to rerun a sim using a specific visitor configuration
void VisitorController::SaveVisitorList(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving as Visitor file...");

		for (int v = 0; v < GVisitorController->Visitors.size(); v++)
		{
			Visitor vx = GVisitorController->Visitors[v];

			file << vx.Name << "," << vx.Type << "," << vx.Rides.maxWaitTime << "," << vx.Rides.preference << "," << vx.StayDuration << "," << vx.ArrivalTime.hours << "," << vx.ArrivalTime.minutes << "," << vx.DepartureTime.hours << "," << vx.DepartureTime.minutes << "," << vx.StayingOnSite << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Unable to save visitor list file." << std::endl;
	}
}


void VisitorController::OutputStatus(std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


int VisitorController::GetLargestValueByType(int parameter)
{
	int result = 0;

	for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
	{
		switch (parameter)
		{
		case kDailyStatsByTypeNoRideAvailable:
			if (DailyStatsByType[t].noRideAvailable > result)
			{
				result = DailyStatsByType[t].noRideAvailable;
			}
			break;
		case kDailyStatsByTypeWaitTimeTooLong:
			if (DailyStatsByType[t].waitTimeTooLong > result)
			{
				result = DailyStatsByType[t].waitTimeTooLong;
			}
			break;
		case kDailyStatsByTypeRideShutdown:
			if (DailyStatsByType[t].rideShutdown > result)
			{
				result = DailyStatsByType[t].rideShutdown;
			}
			break;
		case kDailyStatsByTypeDistanceTravelled:
			if (DailyStatsByType[t].distanceTravelled > result)
			{
				result = DailyStatsByType[t].distanceTravelled;
			}
			break;
		case kDailyStatsByTypeAverageQueueTime:
			if (DailyStatsByType[t].averageQueueTime > result)
			{
				result = DailyStatsByType[t].averageQueueTime;
			}
			break;
		case kDailyStatsByTypeAverageIdleTime:
			if (DailyStatsByType[t].averageIdleTime > result)
			{
				result = static_cast<int>(DailyStatsByType[t].averageIdleTime);
			}
			break;
		case kDailyStatsByTypeAverageRidingTime:
			if (DailyStatsByType[t].averageRidingTime > result)
			{
				result = static_cast<int>(DailyStatsByType[t].averageRidingTime);
			}
			break;
		case kDailyStatsByTypeAverageTravellingTime:
			if (DailyStatsByType[t].averageTravellingTime > result)
			{
				result = static_cast<int>(DailyStatsByType[t].averageTravellingTime);
			}
			break;
		case kDailyStatsHighestRideCountByIndex:
			result = kDailyRideCount - 1;

			for (int t = 0; t < kDailyRideCount; t++)
			{
				if (DailyStats.rideCount[t] != 0)
				{
					result = t;
				}
			}
			break;
		case kDailyStatsHighestRideCountByValue:
			result = 0;

			for (int t = 0; t < kDailyRideCount; t++)
			{
				if (DailyStats.rideCount[t] > result)
				{
					result = DailyStats.rideCount[t];
				}
			}
		}
	}

	return result;
}


void VisitorController::UpdateMinuteStats(std::string current_time)
{
	MinuteDataV m;

	m.time = current_time;

	int Data[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int RideCount = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		Visitor visitor = Visitors[v];

		Data[visitor.ParkStatus]++;

		RideCount += Visitors[v].Rides.count;

		if (Visitors[v].Rides.count > m.maxRides)
		{
			m.maxRides = Visitors[v].Rides.count;
		}

		if (Visitors[v].Rides.count < m.minRides)
		{
			m.minRides = Visitors[v].Rides.count;
		}

		if (visitor.ParkStatus != Constants::ParkStatusOnWay && visitor.ParkStatus != Constants::ParkStatusExited)
		{
			Data[Constants::VisitorsInPark]++;
		}
	}

	m.visitorsInPark = Data[Constants::VisitorsInPark];
	m.onWay          = Data[Constants::ParkStatusOnWay];
	m.atEntrance     = Data[Constants::ParkStatusAtEntrance];
	m.idle           = Data[Constants::ParkStatusIdle];
	m.riding         = Data[Constants::ParkStatusRiding];
	m.queing         = Data[Constants::ParkStatusQueuing];
	m.queingFastPass = Data[Constants::ParkStatusQueuingFastPass];
	m.travelling     = Data[Constants::ParkStatusTravelling];
	m.waiting        = Data[Constants::ParkStatusWaiting];
	m.exited         = Data[Constants::ParkStatusExited];

	if (RideCount != 0 && Visitors.size() != 0)
	{
		m.averageRides = static_cast<int>((float)RideCount / (float)Visitors.size()); // average number of rides per visitor
	}
	else
	{
		m.averageRides = 0;
	}

	Statistics.push_back(m);
}


std::string VisitorController::GetMinuteDataFor(int minute)
{
	return std::to_string(Statistics[minute].visitorsInPark) + "," +
	       std::to_string(Statistics[minute].onWay) + "," +
		   std::to_string(Statistics[minute].atEntrance) + "," +
		   std::to_string(Statistics[minute].idle) + "," +
		   std::to_string(Statistics[minute].riding) + "," +
		   std::to_string(Statistics[minute].queing) + "," +
		   std::to_string(Statistics[minute].queingFastPass) + "," +
		   std::to_string(Statistics[minute].travelling) + "," +
		   std::to_string(Statistics[minute].waiting) + "," +
		   std::to_string(Statistics[minute].exited) + "," +
		   std::to_string(Statistics[minute].averageRides) + "," +
		   std::to_string(Statistics[minute].minRides) + "," + 
		   std::to_string(Statistics[minute].maxRides);
} 


MinuteDataV VisitorController::GetMinuteDataStructFor(int minute)
{
	return Statistics[minute];
}


void VisitorController::CalculateStatistics()
{
	for (int v = 0; v < Visitors.size(); v++)
	{
		DailyStats.totalRides += Visitors[v].Rides.count;
		DailyStats.totalFastPastRides += Visitors[v].Rides.fastPassRides;

		DailyStats.totalIdleTime += Visitors[v].TimeSpent.idle;
		DailyStats.totalQueueTime += Visitors[v].TimeSpent.queuing;
		DailyStats.totalRidingTime += Visitors[v].TimeSpent.riding;
		DailyStats.totalTravellingTime += Visitors[v].TimeSpent.travelling;

		DailyStats.noRideAvailable += Visitors[v].Rides.noRideAvailable;
		DailyStats.waitTimeTooLong += Visitors[v].Rides.waitTimeTooLong;
		DailyStats.rideShutdown += Visitors[v].Rides.rideShutdown;
		DailyStats.distanceTravelled += Visitors[v].Rides.distanceTravelled;

		if (Visitors[v].Rides.count >= 50)
		{
			DailyStats.rideCount[50]++;
		}
		else
		{
			DailyStats.rideCount[Visitors[v].Rides.count]++;

			if (Visitors[v].Rides.count == 0)
			{
				DailyStats.zeroRides++;
			}
		}

		if (Visitors[v].Rides.count > DailyStats.maxRides)
		{
			DailyStats.maxRides = Visitors[v].Rides.count;
		}

		if (Visitors[v].Rides.count < DailyStats.minRides)
		{
			DailyStats.minRides = Visitors[v].Rides.count;
		}

		// == by type ==========================================================================================

		if (Visitors[v].Rides.count > DailyStatsByType[Visitors[v].Type].maxRides)
		{
			DailyStatsByType[Visitors[v].Type].maxRides = Visitors[v].Rides.count;
		}

		if (Visitors[v].Rides.count < DailyStatsByType[Visitors[v].Type].minRides)
		{
			DailyStatsByType[Visitors[v].Type].minRides = Visitors[v].Rides.count;
		}

		if (Visitors[v].Rides.count >= 50)
		{
			DailyStatsByType[Visitors[v].Type].rideCount[50]++;
		}
		else
		{
			DailyStatsByType[Visitors[v].Type].rideCount[Visitors[v].Rides.count]++;

			if (Visitors[v].Rides.count == 0)
			{
				DailyStatsByType[Visitors[v].Type].zeroRides++;
			}
		}

		DailyStatsByType[Visitors[v].Type].totalRides += Visitors[v].Rides.count;
		DailyStatsByType[Visitors[v].Type].totalFastPastRides += Visitors[v].Rides.fastPassRides;

		DailyStatsByType[Visitors[v].Type].totalIdleTime += Visitors[v].TimeSpent.idle;
		DailyStatsByType[Visitors[v].Type].totalQueueTime += Visitors[v].TimeSpent.queuing;
		DailyStatsByType[Visitors[v].Type].totalRidingTime += Visitors[v].TimeSpent.riding;
		DailyStatsByType[Visitors[v].Type].totalTravellingTime += Visitors[v].TimeSpent.travelling;

		DailyStatsByType[Visitors[v].Type].noRideAvailable += Visitors[v].Rides.noRideAvailable;
		DailyStatsByType[Visitors[v].Type].waitTimeTooLong += Visitors[v].Rides.waitTimeTooLong;
		DailyStatsByType[Visitors[v].Type].rideShutdown += Visitors[v].Rides.rideShutdown;
		DailyStatsByType[Visitors[v].Type].distanceTravelled += Visitors[v].Rides.distanceTravelled;

		DailyStatsByType[Visitors[v].Type].typeCount++;
	}

	// =====================================================================================================
	// =====================================================================================================

	DailyStats.averageRides     = static_cast<int>((float)DailyStats.totalRides / (float)Visitors.size());
	DailyStats.averageQueueTime = static_cast<int>((float)DailyStats.totalQueueTime / (float)DailyStats.totalRides);

	DailyStats.averageIdleTime = (float)DailyStats.totalIdleTime / (float)Visitors.size();
	DailyStats.averageRidingTime = (float)DailyStats.totalRidingTime / (float)Visitors.size();
	DailyStats.averageTravellingTime = (float)DailyStats.totalTravellingTime / (float)Visitors.size();
	DailyStats.averageDistanceTravelled = static_cast<int>((float)DailyStats.distanceTravelled / (float)Visitors.size());

	for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
	{
		DailyStatsByType[t].averageRides = static_cast<int>((float)DailyStatsByType[t].totalRides / (float)DailyStatsByType[t].typeCount);
		DailyStatsByType[t].averageQueueTime = static_cast<int>((float)DailyStatsByType[t].totalQueueTime / (float)DailyStatsByType[t].totalRides);

		DailyStatsByType[t].averageIdleTime = (float)DailyStatsByType[t].totalIdleTime / (float)DailyStatsByType[t].typeCount;
		DailyStatsByType[t].averageRidingTime = (float)DailyStatsByType[t].totalRidingTime / (float)DailyStatsByType[t].typeCount;
		DailyStatsByType[t].averageTravellingTime = (float)DailyStatsByType[t].totalTravellingTime / (float)DailyStatsByType[t].typeCount;

		DailyStatsByType[t].averageDistanceTravelled = static_cast<int>((float)DailyStatsByType[t].distanceTravelled / (float)DailyStatsByType[t].typeCount);
	}
}


void VisitorController::ShowStatistics()
{
	std::cout << "\n";

	if (DailyStats.totalRides != 0)
	{
		if (DailyStats.totalFastPastRides != 0)
		{
			std::cout << "Total rides: " << DailyStats.totalRides << " (fp: " << DailyStats.totalFastPastRides << ");  Avg. queue time: " << DailyStats.averageQueueTime << " mins; Avg. idle time: " << DailyStats.averageIdleTime << " mins; Avg travel time: " << DailyStats.averageTravellingTime << " mins" << "\n";
		}
		else
		{
			std::cout << "Total rides: " << DailyStats.totalRides << "; Avg. queue time: " << DailyStats.averageQueueTime << " mins; Avg. idle time: " << DailyStats.averageIdleTime << " mins; Avg. travel time: " << DailyStats.averageTravellingTime << " mins" << "\n";
		}

		std::cout << "Max rides " << DailyStats.maxRides << ", min rides " << DailyStats.minRides << ", average " << DailyStats.averageRides << std::endl;
	}
}