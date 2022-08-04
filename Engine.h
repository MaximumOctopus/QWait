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

	QWaitTypes::Time CurrentTime;
	int TimeParkCloses;
	int TimeRidesClose;

	int UpdateRate = 15;

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

	void ParkStatusEntrance(int);
	void ParkStatusIdle(int);
	void ParkStatusRiding(int);
	void ParkStatusTravelling(int);
	void ParkStatusWaiting(int);

	void ShowLiveStats();

public:

	int VisitorsInPark = 0;

	Engine(FastPassType, int, int, bool, int);

	void Run(bool, bool);	
};