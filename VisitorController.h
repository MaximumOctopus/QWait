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


#pragma once


#include <string>
#include <vector>
#include "Constants.h"
#include "Visitor.h"


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
	std::string time = "";
	int visitorsInPark = 0;
	int onWay = 0;			// number of visitors with this ParkStatus
	int atEntrance = 0;		//
	int idle = 0;			//
	int riding = 0;			//
	int queing = 0;			//
	int queingFastPass = 0; //
	int travelling = 0;		//	
	int waiting = 0;		//
	int exited = 0;			//

	int averageRides = 0;	// average number of rides ridden by all visitors
	int maxRides = 0;       // highest rides ridden by any visitor at this minute of the day
	int minRides = 999;		// lowest rides ridden by any visitor at this minute of the day
};


struct Daily {
	int minRides = 999;
	int maxRides = 0;
	int zeroRides = 0;
	int averageRides = 0;
	int totalRides = 0;
	int totalFastPastRides = 0;

	int totalQueueTime = 0;
	int averageQueueTime = 0;

	int totalIdleTime = 0;
	float averageIdleTime = 0.0f;

	int totalRidingTime = 0;
	float averageRidingTime = 0.0f;

	int totalTravellingTime = 0;
	float averageTravellingTime = 0.0f;

	int noRideAvailable = 0;
	int waitTimeTooLong = 0;
	int rideShutdown = 0;
	int distanceTravelled = 0; // metres

	int averageDistanceTravelled = 0; // metres;

	int rideCount[kDailyRideCount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // each slot represents the number of visitors to ride that many times
};


struct DailyByType {
	int minRides = 999;
	int maxRides = 0;
	int zeroRides = 0;
	int averageRides = 0;
	int totalRides = 0;
	int totalFastPastRides = 0;

	int totalQueueTime = 0;
	int averageQueueTime = 0;

	int totalIdleTime = 0;
	float averageIdleTime = 0.0f;

	int totalRidingTime = 0;
	float averageRidingTime = 0.0f;

	int totalTravellingTime = 0;
	float averageTravellingTime = 0.0f;

	int noRideAvailable = 0;
	int waitTimeTooLong = 0;
	int rideShutdown = 0;
	int distanceTravelled = 0; // metres
	int averageDistanceTravelled = 0; // metres

	int rideCount[kDailyRideCount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // each slot represents the number of visitors to ride that many times
	int typeCount = 0;
};


struct StatsFastPass {
	size_t ticketsEarly = 0;
	size_t ticketsEarlyAll = 0;
};


class VisitorController
{
	std::vector<MinuteDataV> Statistics;

	int VisitorCount;

	bool ShowOutput;

	void SetDefaults();

	int GetType(float);
	TypeConfiguration GetTypeConfiguration(int);

	bool CreateVisitorsFromFileData(std::string);

	void OutputStatus(std::string status);

public:
	 
	int TypeCount[Constants::AvailableVisitorTypes];
	
	Daily DailyStats;
	DailyByType DailyStatsByType[Constants::AvailableVisitorTypes];
	StatsFastPass FastPassStats;

	std::vector<Visitor> Visitors;

	VisitorController(int, bool);

	int LoadVisitorList(std::string);
	void SaveVisitorList(std::string);

	void ShowConfig();
	void Run(bool, std::string, bool, std::string);

	int GetLargestValueByType(int);

	void UpdateMinuteStats(std::string);
	std::string GetMinuteDataFor(int);
	MinuteDataV GetMinuteDataStructFor(int);

	void CalculateStatistics();

	void ShowStatistics();
};