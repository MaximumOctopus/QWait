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
#include "Eatery.h"
#include "RidePricing.h"
#include "QWaitTypes.h"
#include "Ride.h"


const static int kSelectionChoiceCacheCount   = 200;

const static int kGetMaxQueueLength = 0;

const static std::wstring kRideTypeNames[2] = { L"Continuous", L"Show" };


struct Entrance
{
	std::wstring ParkName;

	QWaitTypes::Coords position = { 0, 0 };
	int throughPutMinute = 0; 

	int fastPassType = 0; // 0, none; 1, types 1 and 2 fastpass; 3, type 3 park

	int averageVisitors = 0; // an average visitor value for this park
	int maximumCapacity = 0; // maximum visitor number

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
	bool FoodDrink = false;

	std::vector<int> EntranceQueue;

	int WalkTimeCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)
	int WalkDistanceCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)

	void BuildFromTemplate(ParkTemplate);

	void BuildDistanceCache();
	void BuildClosestCache();
	void BuildSelectionChoiceCache();
	void BuildFastPassList();

	void UpdateThroughput();

	void OutputStatus(std::wstring);

public:

	std::vector<std::wstring> Log;

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
	std::vector<Eatery> Eateries;

	ParkController(bool, bool, const std::wstring);

	bool BuildFromTemplate(const std::wstring);

	void SaveAsTemplate(const std::wstring);

	void AddNewRideContinuous(const std::wstring, int, RideExitType, int, int, int, int, int, int, bool, bool, bool);		// eg it's a small world
	void AddNewRideShow(const std::wstring, int, RideExitType, int, int, int, int, int, int, int, int, bool, bool, bool);	// eg lion king show

	void AddNewRideContinuousWithHours(const std::wstring, int, RideExitType, int, int, int, int, int, int, int, int, int, int, bool, bool, bool);		// eg it's a small world
	void AddNewRideShowWithHours(const std::wstring, int, RideExitType, int, int, int, int, int, int, int, int, int, int, int, int, bool, bool, bool);	// eg lion king show

	void AddFoodDrink(const std::wstring, int, bool, bool, int, int, int, int, int);

	void SetEntrance(const std::wstring, int, int, int, int, int, int, int, int, int, int, int);

	void BuildRides(ParkTemplate, const std::wstring);

	void AddToLog(const std::wstring);

	int BuyTicket(AgeGroup, bool);

	int GetDistanceBetweenInMinutes(int, int);
	int GetDistanceBetweenInMetres(int, int);

	int GetMinuteCount();

	int GetLargestValue(int);

	int GetLargestTicketCategory();

	void ShowConfig();
	void ShowStatistics(int);
	int TotalEaterySpending();

	bool SaveDistanceCache(const std::wstring);

	QWaitTypes::GetEatery GetClosestEatery(QWaitTypes::Coords, int);
};