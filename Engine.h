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


class Engine
{
	bool ShowOutput;

	bool ParkOpen;
	int FastPassMode;

	QWaitTypes::Time CurrentTime;
	int TimeParkCloses;
	int TimeRidesClose;

	void UpdateClock();

	void OutputStatus(std::string);

	QWaitTypes::GetRide GetRide(int);
	int IsRideAvailableCloser(int, int);
	QWaitTypes::FastPass IsFastPassTicketSoon(int, int);

	void PreArrivalFastPass();
	void ProcessFastPassesForVisitor(int);
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

	int VisitorsInPark;

	Engine(int, int, int, bool);

	void Run(bool, bool);	
};