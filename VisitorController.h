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


#pragma once


#include <string>
#include <time.h>
#include <vector>

#include "Constants.h"
#include "Group.h"
#include "Visitor.h"
#include "VisitorController.h"


static const int kDailyStatsByTypeNoRideAvailable = 0;
static const int kDailyStatsByTypeWaitTimeTooLong = 1;
static const int kDailyStatsByTypeRideShutdown = 2;
static const int kDailyStatsByTypeDistanceTravelled = 3; // metres
static const int kDailyStatsByTypeAverageQueueTime = 4;
static const int kDailyStatsByTypeAverageIdleTime = 5;
static const int kDailyStatsByTypeAverageRidingTime = 6;
static const int kDailyStatsByTypeAverageTravellingTime = 7;
static const int kDailyStatsHighestRideCountByIndex = 8;
static const int kDailyStatsHighestRideCountByValue = 9;
static const int kDailyStatsHighestAverageRideCountByValue = 10;

static const int kDailyRideCount = 51;


struct MinuteDataV {
	std::wstring time = L"";

	int visitorsInPark = 0;

	int onWay = 0;			// number of visitors with this ParkStatus
	int atEntrance = 0;		//
	int idle = 0;			//
	int riding = 0;			//
	int queuing = 0;		//
	int queuingFastPass = 0;//
	int travelling = 0;		//	
	int waiting = 0;		//
	int exited = 0;			//

	int averageRides = 0;	// average number of rides ridden by all visitors
	int maxRides = 0;       // highest rides ridden by any visitor at this minute of the day
	int minRides = 999;		// lowest rides ridden by any visitor at this minute of the day
};


struct DailyStatistics {
	int minRides = 999;
	int maxRides = 0;
	int zeroRides = 0;
	int averageRides = 0;
	int totalRides = 0;
	int totalFastPastRides = 0;

	int queueTimePerRideTime = 0;

	int totalQueueTime = 0;
	int averageQueueTime = 0;

	int totalIdleTime = 0;
	double averageIdleTime = 0.0f;

	int totalRidingTime = 0;
	double averageRidingTime = 0.0f;

	int totalTravellingTime = 0;
	double averageTravellingTime = 0.0f;

	int totalWaitingTime = 0;
	double averageWaitingTime = 0.0f;

	int noRideAvailable = 0;
	int waitTimeTooLong = 0;
	int rideShutdown = 0;
	int distanceTravelled = 0; // metres
	int averageDistanceTravelled = 0; // metres

	int rideCount[kDailyRideCount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // each slot represents the number of visitors to ride that many times
	int typeCount = 0;

	int totalSpend = 0;
	double totalSpendPerRide = 0.0f;
};


struct StatsFastPass {
	size_t ticketsEarly = 0;
	size_t ticketsEarlyAll = 0;
};


class VisitorController
{
	std::vector<MinuteDataV> Statistics;

	int ExpectedVisitorCount = 0;

	ParkTemplate TemplateID = ParkTemplate::WDWAnimalKingdom;
	int TemplateIDint = 1;

	bool ShowOutput = true;

	bool GetVisitorName = false;

	void SetDefaults();

	GroupType GetGroupType(double);

	bool CreateGroupFromFileData(const std::wstring);

	void OutputStatus(const std::wstring);

public:

	// the number of visitors and groups created, maximum values.
	// not be altered once set. doesn't represent current number of 
	// groups or visitors in park
	int VisitorCountCreated = 0;
	int GroupCountCreated = 0;
	 
	int GroupTypeCount[Constants::AvailableGroupTypes];
	int TypeCount[Constants::AvailableVisitorTypes];
	
	DailyStatistics DailyStats;
	DailyStatistics DailyStatsByType[Constants::AvailableVisitorTypes];
	StatsFastPass FastPassStats;

	DailyStatistics DailyStatsByGroup[Constants::AvailableGroupTypes];

	std::vector<Group> Groups;

	VisitorController(int, bool, ParkTemplate, bool);

	int LoadVisitorList(const std::wstring);
	void SaveVisitorList(const std::wstring);

	void ShowConfig();
	void Run(bool, const std::wstring, bool, const std::wstring);

	int GetCurrentVisitorCount();

	int GetLargestValueByType(int);

	int TotalSpending();

	void UpdateMinuteStats(const std::wstring);
	std::wstring GetMinuteDataFor(int);
	MinuteDataV GetMinuteDataStructFor(int);

	void CalculateDemographics();

	void CalculateStatistics();

	void ShowStatistics();
};