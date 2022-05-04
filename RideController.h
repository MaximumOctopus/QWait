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


#include "Constants.h"
#include "QWaitTypes.h"
#include "Ride.h"
#include <string>
#include <vector>


const static int kSelectionChoiceCacheCount   = 200;

const static int kGetMaxQueueLength = 0;

const static std::string kRideTypeNames[2] = { "Continuous", "Show" };


struct Entrance
{
	QWaitTypes::Coords position = { 0, 0 };
	int throughPutMinute = 0; 

	int fastPassType = 0; // 0, none; 1, types 1 and 2 fastpass; 3, type 3 park

	int averageVisitors = 0; // an average visitor value for this park
};


// stores the distance from the this object to the ride specified
struct DistanceInfo {
	int ride = 0;
	int distanceTo = 0;
};


class RideController
{
	bool ShowOutput;

	std::vector<int> EntranceQueue;

	int WalkTimeCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)
	int WalkDistanceCache[Constants::MaxRideCount][Constants::MaxRideCount]; // not dynamic ;)

	void BuildFromTemplate(int);

	void BuildDistanceCache();
	void BuildClosestCache();
	void BuildSelectionChoiceCache();
	void BuildFastPassList();

	void UpdateThroughput();

	void OutputStatus(std::string);

public:

	std::vector<std::string> Log;

	std::vector<int> FastPassType[3];

	int FastPassMode;

	std::string ParkName;
	int TheoreticalHourThroughputTotal;
	int TheoreticalHourThroughputTotalDay;

	size_t EntranceQueueSize();
	void AddToEntranceQueue(int);
	int RemoveFromEntranceQueue();

	int SelectionChoiceCache[Constants::AvailableVisitorTypes][kSelectionChoiceCacheCount];

	Entrance entrance;

	int Minutes; // how many minutes of data are stored

	std::vector<Ride> Rides;

	RideController(bool, std::string);

	bool BuildFromTemplate(std::string);

	void SaveAsTemplate(std::string);

	void AddNewRideContinuous(std::string, int, int, int, int, int, int, int);		// eg it's a small world
	void AddNewRideShow(std::string, int, int, int, int, int, int, int, int, int);	// eg lion king show

	void AddNewRideContinuousWithHours(std::string, int, int, int, int, int, int, int, int, int, int, int);		// eg it's a small world
	void AddNewRideShowWithHours(std::string, int, int, int, int, int, int, int, int, int, int, int, int, int);	// eg lion king show

	void SetEntrance(std::string, int, int, int, int);

	void BuildRides(int, std::string);

	void AddToLog(std::string LogEntry);

	int GetDistanceBetweenInMinutes(int, int);
	int GetDistanceBetweenInMetres(int, int);

	int GetMinuteCount();

	int GetLargestValue(int);

	void ShowConfig();
	void ShowStatistics(int);
};