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
#include "QWaitTypes.h"


class Engine
{
	bool ShowOutput;

	bool ParkOpen;
	FastPassType FastPassMode;

	bool HandleFoodDrink = false;

	QWaitTypes::Time CurrentTime;
	int TimeParkCloses;
	int TimeRidesClose;

	int Temperature = 20;

	int UpdateRate = 15; // every n minutes

	void UpdateClock();

	void OutputStatus(const std::wstring);

	QWaitTypes::GetRide GetRide(int);
	int IsRideAvailableCloser(int, int);
	QWaitTypes::FastPass IsFastPassTicketSoon(int, int);

	void PreArrivalFastPass();
	void ProcessFastPassesForGroup(int);
	QWaitTypes::FastPass GetFastPassRide(int, QWaitTypes::Time, QWaitTypes::Time);
	void GetReplacementFastPassRide(int);

	void EntranceProcessQueue();

	void ProcessRide(int);

	void RideContinuousProcessQueue(int);
	void RideShowProcessQueue(int);
	void RideEmptyQueue(int);

	void EateryProcessQueue(int);
	void EateryEmptyQueue(int);

	void ParkStatusEntrance(int);
	void ParkStatusIdle(int);
	void ParkStatusRiding(int);
	void ParkStatusTravellingRide(int);
	void ParkStatusWaiting(int);

	void ParkStatusTravellingFood(int);
	void ParkStatusEating(int);

	void ShowLiveStats();

public:

	int VisitorsInPark = 0;

	Engine(FastPassType, int, int, bool, int, bool, int);

	void Run(bool, bool);	
};