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
#include <vector>


#include "Constants.h"
#include "RidePricing.h"
#include "QWaitTypes.h"
#include "Ride.h"


const static int kSelectionChoiceCacheCount   = 200;

const static int kGetMaxQueueLength = 0;

const static std::string kRideTypeNames[2] = { "Continuous", "Show" };


struct Entrance
{
	std::string ParkName;

	QWaitTypes::Coords position = { 0, 0 };
	int throughPutMinute = 0; 

	int fastPassType = 0; // 0, none; 1, types 1 and 2 fastpass; 3, type 3 park

	int averageVisitors = 0; // an average visitor value for this park

	RidePricing Prices;

	TicketsPurchased Tickets;
};


// stores the distance from the this object to the ride specified
struct DistanceInfo {
	int ride = 0;
	int distanceTo = 0;
};


class ParkController
{
	bool ShowOutput;

	std::vector<int> EntranceQueue;

	int WalkTimeCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)
	int WalkDistanceCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)

	void BuildFromTemplate(ParkTemplate);

	void BuildDistanceCache();
	void BuildClosestCache();
	void BuildSelectionChoiceCache();
	void BuildFastPassList();

	void UpdateThroughput();

	void OutputStatus(std::string);

public:

	std::vector<std::string> Log;

	std::vector<int> FastPassTypes[3];

	FastPassType FastPassMode;

	int TheoreticalHourThroughputTotal;
	int TheoreticalHourThroughputTotalDay;

	size_t EntranceQueueSize();
	void AddToEntranceQueue(int);
	int RemoveFromEntranceQueue();

	int SelectionChoiceCache[Constants::AvailableVisitorTypes][kSelectionChoiceCacheCount];

	Entrance entrance;

	int Minutes; // how many minutes of data are stored

	std::vector<Ride> Rides;

	ParkController(bool, const std::string);

	bool BuildFromTemplate(const std::string);

	void SaveAsTemplate(const std::string);

	void AddNewRideContinuous(const std::string, int, RideExitType, int, int, int, int, int, int, bool, bool, bool);		// eg it's a small world
	void AddNewRideShow(const std::string, int, RideExitType, int, int, int, int, int, int, int, int, bool, bool, bool);	// eg lion king show

	void AddNewRideContinuousWithHours(const std::string, int, RideExitType, int, int, int, int, int, int, int, int, int, int, bool, bool, bool);		// eg it's a small world
	void AddNewRideShowWithHours(const std::string, int, RideExitType, int, int, int, int, int, int, int, int, int, int, int, int, bool, bool, bool);	// eg lion king show

	void SetEntrance(const std::string, int, int, int, int, int, int, int, int, int, int);

	void BuildRides(ParkTemplate, const std::string);

	void AddToLog(const std::string);

	int BuyTicket(AgeGroup, bool);

	int GetDistanceBetweenInMinutes(int, int);
	int GetDistanceBetweenInMetres(int, int);

	int GetMinuteCount();

	int GetLargestValue(int);

	int GetLargestTicketCategory();

	void ShowConfig();
	void ShowStatistics(int);

	bool SaveDistanceCache(const std::string);
};