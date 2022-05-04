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
#include "QWaitTypes.h"
#include "Visitor.h"


static const int kNoSelectedFastPassTicket = -1;

static const int kRideTypeContinuous = 0;
static const int kRideTypeShow = 1;

static const int kClosestCacheSize = 5;


struct Operation {
	int rideType = 0;
	std::string name = "";

	QWaitTypes::Coords position; // x position in 2d map

	int rideLength = 0;				 // minutes

	bool isOpen = 0;

	bool isShutdown = 0;			 // after closed for the day

	QWaitTypes::Time open = { 0, 0 };
	QWaitTypes::Time close = { 0, 0 };

	int ShowStartTime = 0; // for show type rides only, every x minutes

	int operationHours = 0;

	int Popularity = 0;            //
	//bool ChildValid;           // unused for now
	//bool AdultValid;           // unused for now
};


struct MinuteData {
	size_t queueSize = 0;
	size_t queueSizeFastPass = 0;
	int riders = 0;
	int waitTimeMinutes = 0;
	int waitTimeMinutesFastPass = 0;
};


struct DailyData {
	size_t maxQueueLength = 0;
	int totalRiders = 0;
};


struct FastPassSystem {
	int mode = 0;					// 0 - inactive, or type x
	int percentage = 0;

	int ticketsLeftFoHour[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};


struct Capacity {
	int totalPerHour = 0;			// visitors per hour	

	float perMinuteTotal = 0;		// HourlyThroughput / 60
	float perMinute = 0;			// HourlyThroughput / 60
	int perMinuteI = 0;				// integer version of above

	float perMinuteFastPass = 0;	// HourlyThroughput / 60
	int perMinuteIFastPass = 0;		// integer version of above

	int showCapacity = 0;
	int showCapacityFastPass = 0;
};


class Ride
{
	std::vector<int> Queue;
	std::vector<int> QueueFastPass;

	std::vector<MinuteData> Statistics;

	void InitFastPass();
	int ResetFastPassTicketsLeft();

	void ConfigureThroughput(int);

public:

	Operation RideOperation;

	int ClosestCache[kClosestCacheSize]; // the n closest rides to this one
	
	int CurrentRiders;			

	FastPassSystem FastPass;

	DailyData DailyStatistics;

	Capacity RideThroughput;

	Ride(int, std::string, int, int, int, int, int, int, int, QWaitTypes::Time, QWaitTypes::Time, int);

	void Close();

	std::string GetUniqueReference();

	void ConfigureShowThroughput(int, int);

	void AddToQueue(int);
	int RemoveFromQueue();

	void AddToQueueFastPass(int);
	int RemoveFromQueueFastPass();

	size_t QueueSize();
	size_t QueueSizeFastPass();

	float WaitTime(int);

	int GetFastPassTicket(QWaitTypes::Time, QWaitTypes::Time);
	int ViewFastPassTicket(QWaitTypes::Time, QWaitTypes::Time);

	size_t GetMinuteStatsCount();

	std::string GetMinuteStatFor(int);
	void UpdateMinuteStats();

	void UpdateDailyStatistics();
};