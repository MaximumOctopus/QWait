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
#include <random>
#include <sstream>
#include <time.h>

#include "Constants.h"
#include "Group.h"
#include "Names.h"
#include "QWaitTypes.h"
#include "ReportUtility.h"
#include "Utility.h"
#include "Visitor.h"
#include "VisitorController.h"


VisitorController* GVisitorController;


VisitorController::VisitorController(int count, bool show_output, bool food_drink, ParkTemplate park_template, bool visitor_name)
{
	GetVisitorName = visitor_name;

	TemplateID = park_template;
	TemplateIDint = QWaitTypes::ParkTemplateToInt(park_template);

	ExpectedVisitorCount = count;

	ShowOutput = show_output;

	FoodDrink = food_drink;

	SetDefaults();
}


void VisitorController::SetDefaults()
{
	for (int x = 0; x < kDailyRideCount; x++)
	{
		DailyStats.rideCount[x] = 0;
	}
}


GroupType VisitorController::GetGroupType(double random)
{
	if (random < kGroupTypeCoeff[TemplateIDint][0])
	{
		return GroupType::Family;
	}
	else if (random < kGroupTypeCoeff[TemplateIDint][1])
	{
		return GroupType::AdultCouple;
	}
	else if (random < kGroupTypeCoeff[TemplateIDint][2])
	{
		return GroupType::AdultGroup;
	}

	return GroupType::Single;
}


void VisitorController::Run(bool save_visitor_list, const std::wstring save_file_name, bool load_visitor_list, const std::wstring load_file_name)
{
	if (load_visitor_list)
	{
		OutputStatus(L"Loading Visitor data from \"" + load_file_name + L"\"...");
	}
	else
	{
		OutputStatus(L"Generating ~" + std::to_wstring(ExpectedVisitorCount) + L" visitors...");
	}

	if (load_visitor_list)
	{
		int count = LoadVisitorList(load_file_name);

		OutputStatus(L"Loaded " + std::to_wstring(count) + L" visitors.");
	}
	else
	{
		srand((unsigned)time(NULL));

		int visitor_count = 0;

		while (visitor_count < ExpectedVisitorCount)
		{
			std::random_device rd;
			std::default_random_engine eng(rd());
			std::uniform_real_distribution<double> distr(0.0, 1.0);

			GroupType group_type = GetGroupType(distr(eng));

			Group group(group_type, TemplateIDint);

			Groups.push_back(group);

			switch (group_type)
			{
			case GroupType::Family:
				GroupTypeCount[0]++;
				break;
			case GroupType::AdultCouple:
				GroupTypeCount[1]++;
				break;
			case GroupType::AdultGroup:
				GroupTypeCount[2]++;
				break;
			case GroupType::Single:
				GroupTypeCount[3]++;
				break;
			}

			visitor_count += group.Visitors.size();
		}

		/*

		if (GetVisitorName)
		{
			Names NameController;

			if (NameController.LoadFirstNames && NameController.LoadLastNames)
			{
				for (int v = 0; v < Visitors.size(); v++)
				{
					Visitors[v].Name = NameController.GetRandomName();
				}
			}
		}*/
	}

	if (save_visitor_list)
	{
		SaveVisitorList(save_file_name);
	}

	CalculateDemographics();

	VisitorCountCreated = GetCurrentVisitorCount();
	GroupCountCreated = Groups.size();

	ShowConfig();
}


void VisitorController::CalculateDemographics()
{
	for (int g = 0; g < Groups.size(); g++)
	{
		for (int v = 0; v < Groups[g].Visitors.size(); v++)
		{
			TypeCount[Groups[g].Visitors[v].Configuration.TypeInt]++;
		}
	}
}


void VisitorController::ShowConfig()
{
	if (ShowOutput)
	{
		std::wcout << L"Family       :    " << Utility::PadRight(GroupTypeCount[0], 6) << L" " << ReportUtility::BarGraph(static_cast<int>(((double)GroupTypeCount[0] / (double)GroupCountCreated) * 100.0)) << "\n";
		std::wcout << L"Adult Couple :    " << Utility::PadRight(GroupTypeCount[1], 6) << L" " << ReportUtility::BarGraph(static_cast<int>(((double)GroupTypeCount[1] / (double)GroupCountCreated) * 100.0)) << "\n";
		std::wcout << L"Adult Group  :    " << Utility::PadRight(GroupTypeCount[2], 6) << L" " << ReportUtility::BarGraph(static_cast<int>(((double)GroupTypeCount[2] / (double)GroupCountCreated) * 100.0)) << "\n";
		std::wcout << L"Single       :    " << Utility::PadRight(GroupTypeCount[3], 6) << L" " << ReportUtility::BarGraph(static_cast<int>(((double)GroupTypeCount[3] / (double)GroupCountCreated) * 100.0)) << "\n";
		std::wcout << L"=========================\n";
		std::wcout << L"      Total  :    " << Utility::PadRight(GroupCountCreated, 6) << "\n\n";

		for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
		{
			int pc = static_cast<int>(((double)TypeCount[t] / (double)VisitorCountCreated) * 100.0);

			std::wcout << L"Type         : " << t << "  " << Utility::PadRight(TypeCount[t], 6) << L" " << ReportUtility::BarGraph(pc) << "\n";
		}                            
		
		std::wcout << L"=========================\n";
		std::wcout << L"      Total  : " << Utility::PadRight(VisitorCountCreated, 9) << "\n\n";

		std::wcout << std::endl;
	}
}


void VisitorController::OutputStatus(const std::wstring status)
{
	if (ShowOutput)
	{
		std::wcout << status << std::endl;
	}
}


int VisitorController::GetCurrentVisitorCount()
{
	int count = 0;

	for (int g = 0; g < Groups.size(); g++)
	{
		count += Groups[g].Visitors.size();
	}

	return count;
}


// =================================================================================================================
// == Spending =====================================================================================================
// =================================================================================================================


int VisitorController::TotalSpending()
{
	int total = 0;

	for (int g = 0; g < Groups.size(); g++)
	{
		for (int v = 0; v < Groups[g].Visitors.size(); v++)
		{
			total += Groups[g].Visitors[v].Configuration.MoneySpent;
		}
	}

	return total;
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
		case kDailyStatsByTypeEateryQueueTooLong:
			if (DailyStatsByType[t].eateryQueueTooLong > result)
			{
				result = DailyStatsByType[t].eateryQueueTooLong;
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


void VisitorController::UpdateMinuteStats(const std::wstring current_time)
{
 	MinuteDataV m;

	m.time = current_time;

	int Data[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int RideCount = 0;

	for (int g = 0; g < Groups.size(); g++)
	{
		for (int v = 0; v < Groups[g].Visitors.size(); v++)
		{
			Visitor visitor = Groups[g].Visitors[v];

			switch (visitor.ParkStatus)
			{
				case VisitorParkStatus::OnWay:
					Data[Constants::ParkStatusOnWay]++;
					break;
				case VisitorParkStatus::AtEntrance:
					Data[Constants::ParkStatusAtEntrance]++;
					break;
				case VisitorParkStatus::Idle:
					Data[Constants::ParkStatusIdle]++;
					break;
				case VisitorParkStatus::Riding:
					Data[Constants::ParkStatusRiding]++;
					break;
				case VisitorParkStatus::Queuing:
					Data[Constants::ParkStatusQueuing]++;
					break;
				case VisitorParkStatus::QueuingFastPass:
					Data[Constants::ParkStatusQueuingFastPass]++;
					break;
				case VisitorParkStatus::QueuingFood:
					Data[Constants::ParkStatusQueuingFoodDrink]++;
					break;
				case VisitorParkStatus::TravellingRide:
					Data[Constants::ParkStatusTravelling]++;
					break;
				case VisitorParkStatus::Waiting:
					Data[Constants::ParkStatusWaiting]++;
					break;
				case VisitorParkStatus::TravellingFood:
					Data[Constants::ParkStatusTravellingFoodDrink]++;
					break;
				case VisitorParkStatus::Eating:
					Data[Constants::ParkStatusEating]++;
					break;
				case VisitorParkStatus::WaitingForOthersInParty:
					Data[Constants::ParkStatusWaitingForOthersInParty]++;
					break;
				case VisitorParkStatus::Exited:
					Data[Constants::ParkStatusExited]++;
					break;
			}

			RideCount += visitor.Rides.count;

			if (visitor.Rides.count > m.maxRides)
			{
				m.maxRides = visitor.Rides.count;
			}

			if (visitor.Rides.count < m.minRides)
			{
				m.minRides = visitor.Rides.count;
			}

			if (visitor.ParkStatus != VisitorParkStatus::OnWay && visitor.ParkStatus != VisitorParkStatus::Exited)
			{
				Data[Constants::VisitorsInPark]++;
			}
		}
	}

	m.visitorsInPark  = Data[Constants::VisitorsInPark];
	m.onWay           = Data[Constants::ParkStatusOnWay];
	m.atEntrance      = Data[Constants::ParkStatusAtEntrance];
	m.idle            = Data[Constants::ParkStatusIdle];
	m.riding          = Data[Constants::ParkStatusRiding];
	m.queuing         = Data[Constants::ParkStatusQueuing];
	m.queuingFastPass = Data[Constants::ParkStatusQueuingFastPass];
	m.queuingFood     = Data[Constants::ParkStatusQueuingFoodDrink];
	m.travelling      = Data[Constants::ParkStatusTravelling];
	m.waiting         = Data[Constants::ParkStatusWaiting];
	m.travellingFood  = Data[Constants::ParkStatusTravellingFoodDrink];
	m.eating          = Data[Constants::ParkStatusEating];
	m.exited          = Data[Constants::ParkStatusExited];

	if (RideCount != 0 && VisitorCountCreated != 0)
	{
		m.averageRides = static_cast<int>((double)RideCount / (double)VisitorCountCreated); // average number of rides per visitor
	}
	else
	{
		m.averageRides = 0;
	}

	Statistics.push_back(m);
}


std::wstring VisitorController::GetMinuteDataFor(int minute)
{
	return std::to_wstring(Statistics[minute].visitorsInPark) + L"," +
	       std::to_wstring(Statistics[minute].onWay) + L"," +
		   std::to_wstring(Statistics[minute].atEntrance) + L"," +
		   std::to_wstring(Statistics[minute].idle) + L"," +
		   std::to_wstring(Statistics[minute].riding) + L"," +
		   std::to_wstring(Statistics[minute].queuing) + L"," +
		   std::to_wstring(Statistics[minute].queuingFastPass) + L"," +
		   std::to_wstring(Statistics[minute].travelling) + L"," +
		   std::to_wstring(Statistics[minute].waiting) + L"," +
		   std::to_wstring(Statistics[minute].exited) + L"," +
		   std::to_wstring(Statistics[minute].averageRides) + L"," +
		   std::to_wstring(Statistics[minute].minRides) + L"," +
		   std::to_wstring(Statistics[minute].maxRides);
} 


MinuteDataV VisitorController::GetMinuteDataStructFor(int minute)
{
	return Statistics[minute];
}


void VisitorController::CalculateStatistics(int eatery_spending)
{
	for (int g = 0; g < Groups.size(); g++)
	{
		int GroupType = Groups[g].Configuration.GetTypeToInt();

		for (int v = 0; v < Groups[g].Visitors.size(); v++)
		{
			Visitor visitor = Groups[g].Visitors[v];

			DailyStats.totalRides += visitor.Rides.count;
			DailyStats.totalFastPastRides += visitor.Rides.fastPassRides;

			DailyStats.totalIdleTime += visitor.TimeSpent.idle;
			DailyStats.totalQueueTime += visitor.TimeSpent.queuing;
			DailyStats.totalRidingTime += visitor.TimeSpent.riding;
			DailyStats.totalTravellingTime += visitor.TimeSpent.travelling;
			DailyStats.totalWaitingTime += visitor.TimeSpent.waiting;

			DailyStats.noRideAvailable += visitor.Rides.noRideAvailable;
			DailyStats.waitTimeTooLong += visitor.Rides.waitTimeTooLong;
			DailyStats.rideShutdown += visitor.Rides.rideShutdown;
			DailyStats.distanceTravelled += visitor.Rides.distanceTravelled;

			if (FoodDrink)
			{
				DailyStats.eateryQueueTooLong += visitor.Rides.eateryQueueTooLong;

				DailyStats.totalEatingTime += visitor.TimeSpent.eating;
				DailyStats.totalQueueFoodTime += visitor.TimeSpent.queuingFood;
				DailyStats.totalTravellingFoodTime += visitor.TimeSpent.travellingFood;

				DailyStats.totalEateriesVisited += visitor.Rides.eateriesVisited;
			}

			if (visitor.Rides.count >= 50)
			{
				DailyStats.rideCount[50]++;
			}
			else
			{
				DailyStats.rideCount[visitor.Rides.count]++;

				if (visitor.Rides.count == 0)
				{
					DailyStats.zeroRides++;
				}
			}

			if (visitor.Rides.count > DailyStats.maxRides)
			{
				DailyStats.maxRides = visitor.Rides.count;
			}

			if (visitor.Rides.count < DailyStats.minRides)
			{
				DailyStats.minRides = visitor.Rides.count;
			}

			// =====================================================================================================
			// == by group =========================================================================================
			// =====================================================================================================

			if (visitor.Rides.count > DailyStatsByGroup[GroupType].maxRides)
			{
				DailyStatsByGroup[GroupType].maxRides = visitor.Rides.count;
			}

			if (visitor.Rides.count < DailyStatsByGroup[GroupType].minRides)
			{
				DailyStatsByGroup[GroupType].minRides = visitor.Rides.count;
			}

			if (visitor.Rides.count >= 50)
			{
				DailyStatsByGroup[GroupType].rideCount[50]++;
			}
			else
			{
				DailyStatsByGroup[GroupType].rideCount[visitor.Rides.count]++;

				if (visitor.Rides.count == 0)
				{
					DailyStatsByGroup[GroupType].zeroRides++;
				}
			}

			DailyStatsByGroup[GroupType].totalRides += visitor.Rides.count;
			DailyStatsByGroup[GroupType].totalFastPastRides += visitor.Rides.fastPassRides;

			DailyStatsByGroup[GroupType].totalIdleTime += visitor.TimeSpent.idle;
			DailyStatsByGroup[GroupType].totalQueueTime += visitor.TimeSpent.queuing;
			DailyStatsByGroup[GroupType].totalRidingTime += visitor.TimeSpent.riding;
			DailyStatsByGroup[GroupType].totalTravellingTime += visitor.TimeSpent.travelling;
			DailyStatsByGroup[GroupType].totalWaitingTime += visitor.TimeSpent.waiting;

			DailyStatsByGroup[GroupType].noRideAvailable += visitor.Rides.noRideAvailable;
			DailyStatsByGroup[GroupType].waitTimeTooLong += visitor.Rides.waitTimeTooLong;
			DailyStatsByGroup[GroupType].rideShutdown += visitor.Rides.rideShutdown;
			DailyStatsByGroup[GroupType].distanceTravelled += visitor.Rides.distanceTravelled;
			DailyStatsByGroup[GroupType].eateryQueueTooLong += visitor.Rides.eateryQueueTooLong;

			DailyStatsByGroup[GroupType].typeCount++;

			// =====================================================================================================
			// == by type ==========================================================================================
			// =====================================================================================================

			if (visitor.Rides.count > DailyStatsByType[visitor.Configuration.TypeInt].maxRides)
			{
				DailyStatsByType[visitor.Configuration.TypeInt].maxRides = visitor.Rides.count;
			}

			if (visitor.Rides.count < DailyStatsByType[visitor.Configuration.TypeInt].minRides)
			{
				DailyStatsByType[visitor.Configuration.TypeInt].minRides = visitor.Rides.count;
			}

			if (visitor.Rides.count >= 50)
			{
				DailyStatsByType[visitor.Configuration.TypeInt].rideCount[50]++;
			}
			else
			{
				DailyStatsByType[visitor.Configuration.TypeInt].rideCount[visitor.Rides.count]++;

				if (visitor.Rides.count == 0)
				{
					DailyStatsByType[visitor.Configuration.TypeInt].zeroRides++;
				}
			}

			DailyStatsByType[visitor.Configuration.TypeInt].totalRides += visitor.Rides.count;
			DailyStatsByType[visitor.Configuration.TypeInt].totalFastPastRides += visitor.Rides.fastPassRides;

			DailyStatsByType[visitor.Configuration.TypeInt].totalIdleTime += visitor.TimeSpent.idle;
			DailyStatsByType[visitor.Configuration.TypeInt].totalQueueTime += visitor.TimeSpent.queuing;
			DailyStatsByType[visitor.Configuration.TypeInt].totalRidingTime += visitor.TimeSpent.riding;
			DailyStatsByType[visitor.Configuration.TypeInt].totalTravellingTime += visitor.TimeSpent.travelling;
			DailyStatsByType[visitor.Configuration.TypeInt].totalWaitingTime += visitor.TimeSpent.waiting;

			DailyStatsByType[visitor.Configuration.TypeInt].noRideAvailable += visitor.Rides.noRideAvailable;
			DailyStatsByType[visitor.Configuration.TypeInt].waitTimeTooLong += visitor.Rides.waitTimeTooLong;
			DailyStatsByType[visitor.Configuration.TypeInt].rideShutdown += visitor.Rides.rideShutdown;
			DailyStatsByType[visitor.Configuration.TypeInt].distanceTravelled += visitor.Rides.distanceTravelled;
			DailyStatsByType[visitor.Configuration.TypeInt].eateryQueueTooLong += visitor.Rides.eateryQueueTooLong;

			DailyStatsByType[visitor.Configuration.TypeInt].typeCount++;
		}
	}

	// =====================================================================================================

	DailyStats.averageRides = static_cast<int>((double)DailyStats.totalRides / (double)VisitorCountCreated);
	DailyStats.averageQueueTime = static_cast<int>((double)DailyStats.totalQueueTime / (double)VisitorCountCreated);

	DailyStats.averageDistanceTravelled = static_cast<int>((double)DailyStats.distanceTravelled / (double)VisitorCountCreated);
	DailyStats.averageIdleTime = (double)DailyStats.totalIdleTime / (double)VisitorCountCreated;
	DailyStats.averageRidingTime = (double)DailyStats.totalRidingTime / (double)VisitorCountCreated;
	DailyStats.averageTravellingTime = (double)DailyStats.totalTravellingTime / (double)VisitorCountCreated;
	DailyStats.averageWaitingTime = (double)DailyStats.totalWaitingTime / (double)VisitorCountCreated;

	if (DailyStats.totalRidingTime != 0)
	{
		DailyStats.queueTimePerRideTime = static_cast<int>((double)DailyStats.totalQueueTime / (double)DailyStats.totalRidingTime);
	}

	if (FoodDrink)
	{
		DailyStats.averageQueueFoodTime = (double)DailyStats.totalQueueFoodTime / (double)VisitorCountCreated;
		DailyStats.averageTravellingFoodTime = (double)DailyStats.totalTravellingFoodTime / (double)VisitorCountCreated;
		DailyStats.averageEatingTime = (double)DailyStats.totalEatingTime / (double)VisitorCountCreated;

		DailyStats.averageEateriesVisited = (double)DailyStats.totalEateriesVisited / (double)VisitorCountCreated;

		DailyStats.averageTimePerEateryVisit = DailyStats.averageEatingTime / DailyStats.averageEateriesVisited;
	}

	// =====================================================================================================

	for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
	{
		if (DailyStatsByType[t].typeCount != 0)
		{
			DailyStatsByType[t].averageRides = static_cast<int>((double)DailyStatsByType[t].totalRides / (double)DailyStatsByType[t].typeCount);
			DailyStatsByType[t].averageIdleTime = (double)DailyStatsByType[t].totalIdleTime / (double)DailyStatsByType[t].typeCount;
			DailyStatsByType[t].averageRidingTime = (double)DailyStatsByType[t].totalRidingTime / (double)DailyStatsByType[t].typeCount;
			DailyStatsByType[t].averageTravellingTime = (double)DailyStatsByType[t].totalTravellingTime / (double)DailyStatsByType[t].typeCount;
			DailyStatsByType[t].averageWaitingTime = (double)DailyStatsByType[t].totalWaitingTime / (double)DailyStatsByType[t].typeCount;
			DailyStatsByType[t].averageDistanceTravelled = static_cast<int>((double)DailyStatsByType[t].distanceTravelled / (double)DailyStatsByType[t].typeCount);
		}

		if (DailyStatsByType[t].totalRides != 0)
		{
			DailyStatsByType[t].averageQueueTime = static_cast<int>((double)DailyStatsByType[t].totalQueueTime / (double)DailyStatsByType[t].totalRides);
		}

		if (DailyStatsByType[t].totalRidingTime != 0)
		{
			DailyStatsByType[t].queueTimePerRideTime = static_cast<int>((double)DailyStatsByType[t].totalQueueTime / (double)DailyStatsByType[t].totalRidingTime);
		}
	}

	// =====================================================================================================

	for (int g = 0; g < Constants::AvailableGroupTypes; g++)
	{
		if (DailyStatsByGroup[g].typeCount != 0)
		{
			DailyStatsByGroup[g].averageRides = static_cast<int>((double)DailyStatsByGroup[g].totalRides / (double)DailyStatsByGroup[g].typeCount);
			DailyStatsByGroup[g].averageIdleTime = (double)DailyStatsByGroup[g].totalIdleTime / (double)DailyStatsByGroup[g].typeCount;
			DailyStatsByGroup[g].averageRidingTime = (double)DailyStatsByGroup[g].totalRidingTime / (double)DailyStatsByGroup[g].typeCount;
			DailyStatsByGroup[g].averageTravellingTime = (double)DailyStatsByGroup[g].totalTravellingTime / (double)DailyStatsByGroup[g].typeCount;
			DailyStatsByGroup[g].averageWaitingTime = (double)DailyStatsByGroup[g].totalWaitingTime / (double)DailyStatsByGroup[g].typeCount;
			DailyStatsByGroup[g].averageDistanceTravelled = static_cast<int>((double)DailyStatsByGroup[g].distanceTravelled / (double)DailyStatsByGroup[g].typeCount);
		}

		if (DailyStatsByGroup[g].totalRides != 0)
		{
			DailyStatsByGroup[g].averageQueueTime = static_cast<int>((double)DailyStatsByGroup[g].totalQueueTime / (double)DailyStatsByGroup[g].totalRides);
		}

		if (DailyStatsByGroup[g].totalRidingTime != 0)
		{
			DailyStatsByGroup[g].queueTimePerRideTime = static_cast<int>((double)DailyStatsByGroup[g].totalQueueTime / (double)DailyStatsByGroup[g].totalRidingTime);
		}
	}

	// =====================================================================================================

	if (FoodDrink)
	{
		DailyStats.totalEaterySpend = eatery_spending;
		DailyStats.averageEaterySpend = (double)eatery_spending / (double)VisitorCountCreated;
	}

	DailyStats.totalRideSpend = TotalSpending();

	DailyStats.totalSpendPerRide = (double)DailyStats.totalRideSpend / (double)DailyStats.totalRides;
}


void VisitorController::ShowStatistics()
{
	std::wcout << "\n";

	if (DailyStats.totalRides != 0)
	{
		if (DailyStats.totalFastPastRides != 0)
		{
			std::wcout << "Total rides: " << DailyStats.totalRides << " (fp: " << DailyStats.totalFastPastRides << "); Max rides " << DailyStats.maxRides << ", min rides " << DailyStats.minRides << ", average " << DailyStats.averageRides << "\n";
		}
		else
		{
			std::wcout << "Total rides: " << DailyStats.totalRides << "; Max rides " << DailyStats.maxRides << ", min rides " << DailyStats.minRides << ", average " << DailyStats.averageRides << "\n";
		}

		std::wcout << "Avg.queue time: " << DailyStats.averageQueueTime << " mins; Avg.idle time: " << DailyStats.averageIdleTime << " mins; Avg.travel time : " << DailyStats.averageTravellingTime << " mins; Avg. wait time: " << DailyStats.averageWaitingTime << " mins\n";
		std::wcout << "\n";
		std::wcout << "Total queue time: " << DailyStats.totalQueueTime << " mins; total ride time: " << DailyStats.totalRidingTime << " mins; total travel time: " << DailyStats.totalTravellingTime << " mins\n";
		std::wcout << "Total idle time : " << DailyStats.totalIdleTime << " mins; total waiting time: " << DailyStats.totalWaitingTime << " mins.\n";
		std::wcout << "\n";
		std::wcout << "Queue minutes per ride minute: " << DailyStats.queueTimePerRideTime << "\n";
		std::wcout << "\n";

		if (FoodDrink)
		{
			std::wcout << "Total travel food: " << DailyStats.totalTravellingFoodTime << " mins; total queue food: " << DailyStats.totalQueueFoodTime << " mins; total eat time " << DailyStats.totalEatingTime << " mins\n";
			std::wcout << "Total eateries visited: " << DailyStats.totalEateriesVisited << "\n";
			std::wcout << "\n";
			std::wcout << "Avg. travel food: " << DailyStats.averageTravellingFoodTime << " mins; avg. queue food: " << DailyStats.averageQueueFoodTime << " mins; avg. eat time " << DailyStats.averageEatingTime << " mins\n";
			std::wcout << "Avg. eateries visited: " << DailyStats.averageEateriesVisited << "; avg time per visit: " << DailyStats.averageTimePerEateryVisit << " mins.\n";
			std::wcout << "\n";
		}

		std::wcout << "Ride spending $" << DailyStats.totalRideSpend << "; eatery spending $" << DailyStats.totalEaterySpend << "; total = $" << DailyStats.totalEaterySpend + DailyStats.totalRideSpend << "\n\n"; // it's only $ cos I couldn't get £ to work in cout ;)
	}
}


// =================================================================================================================
// == Data I/O =====================================================================================================
// =================================================================================================================


int VisitorController::LoadVisitorList(const std::wstring file_name)
{
	int count = 0; 

	std::wifstream file(file_name);

	if (file)
	{
		OutputStatus(L"Loading Visitors from file...");

		std::wstring s;

		while (std::getline(file, s))
		{
			if (s != L"")
			{
				if (s.find(L"{group}", 0) == 0)
				{
					CreateGroupFromFileData(s);
				}
				else
				{
					if (GVisitorController->Groups[GVisitorController->Groups.size() - 1].CreateVisitorFromFileData(s))
					{ 
						count++;
					}
				}
			}
		}

		file.close();
	}
	else
	{
		std::wcerr << L"Unable to load visitors from file." << std::endl;
	}

	return count;
}


bool VisitorController::CreateGroupFromFileData(const std::wstring input)
{
	std::wifstream visitorinput(input);
	std::vector<std::wstring> parameters;
	std::wstring parameter;

	while (std::getline(visitorinput, parameter, L','))
	{
		parameters.push_back(parameter);
	}

	if (parameters.size() == 9)
	{
		int Type = 0;
		int TemplateID = 0;
		int StayDuration = 0;
		bool StayingOnSite = false;
		int ArrivalHours = 0;
		int ArrivalMinutes = 0;
		int DepartHours = 0;
		int DepartMinutes = 0;

		try
		{
			Type = stoi(parameters[1]);

			TemplateID = stof(parameters[2]);
			StayDuration = stof(parameters[3]);
			StayingOnSite = stoi(parameters[4]);

			ArrivalHours = stoi(parameters[5]);
			ArrivalMinutes = stoi(parameters[6]);
			DepartHours = stoi(parameters[7]);
			DepartMinutes = stoi(parameters[8]);

			Group group(Type, TemplateID, StayDuration, StayingOnSite, ArrivalHours, ArrivalMinutes, DepartHours, DepartMinutes);

			Groups.push_back(group);

			return true;
		}
		catch (...)
		{
			std::wcerr << L"There appears to be some dodgy data in this input row:" << "\n";
			std::wcerr << L"    \"" << input << L"\"" << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Invalid number of parameters in this visitor data (should be 9): " << parameters.size() << "\n";
		std::wcerr << L"    \"" << input << L"\"" << std::endl;
	}

	return false;
}


// saves the randomly generated visitors (without any stats or sim information)
// this is useful if you want to rerun a sim using a specific visitor configuration
void VisitorController::SaveVisitorList(const std::wstring file_name)
{
	std::wofstream file(file_name);

	if (file)
	{
		OutputStatus(L"Saving as Visitor file...");

		for (int g = 0; g < Groups.size(); g++)
		{
			Group gx = Groups[g];

			file << L"{group}," << gx.Configuration.GetTypeToInt() << L"," << gx.Configuration.templateID << L"," << gx.Configuration.stayDuration << L"," << gx.Configuration.stayingOnSite << L"," << gx.Configuration.arrivalTime.hours << L"," << gx.Configuration.arrivalTime.minutes << L"," << gx.Configuration.departureTime.hours << L"," << gx.Configuration.departureTime.minutes << "\n";

			for (int v = 0; v < Groups[g].Visitors.size(); v++)
			{
				Visitor vx = Groups[g].Visitors[v];

				file << vx.Configuration.Name << "," << vx.Configuration.TypeInt << "," << vx.Configuration.GetAgeAsInt() << "," << vx.Rides.maxWaitTime << "," << vx.Rides.preference << "\n";
			}
		}

		file.close();
	}
	else
	{
		std::cerr << L"Unable to save Visitor list file." << std::endl;
	}
}