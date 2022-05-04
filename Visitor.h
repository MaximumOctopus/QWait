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


#include <vector>
#include "Constants.h"
#include "QWaitTypes.h"


const static int kNoCurrentRide = -1;
const static int kNoDestinationRide = -1;
const static int kNoFastPass = -1;
const static int kNoSelectedRide = -1;
const static int kNoSelectedType = -1;
const static int kNotValidVisitor = -1;
const static int kLocationExitedPark = -1;


struct TypeConfiguration {
	std::string name = "";

	int Type = 0;

	float maxWaitTime = 0.0f;
	float preference = 0.0f;

	int stayDuration = 0; // hours

	bool stayingOnSite = false;

	QWaitTypes::Time arrivalTime = { 0, 0 };
	QWaitTypes::Time departureTime = { 0, 0 };
};


struct MinuteDataVI {
	QWaitTypes::Coords position = { 0, 0 };
	int parkStatus = 0;
};


struct TimeStats {
	int idle = 0;
	int riding = 0;
	int travelling = 0;
	int queuing = 0;
};


struct RideStats {
	float maxWaitTime = 0.0f;    // max wait time for a ride (minutes)
	float preference = 0.0f;     // scale from 0 = rides only, 1 = attractions only (actually high priority to low)
	int current = 0;

	int count = 0;

	int noRideAvailable = 0;
	int waitTimeTooLong = 0;
	int rideShutdown = 0;
	int distanceTravelled = 0; // metres
	int noFastPassRideForMe = 0;

	float shortestQueue = 0.0f;
	float longestQueue = 0.0f;

	float shortestQueueFastPass = 0.0f;
	float longestQueueFastPass = 0.0f;

	int fastPassRides = 0;

	int timeLeft = 0;     // minutes left on the current ride
};


struct Travel {
	QWaitTypes::Coords from = { 0, 0 };
	QWaitTypes::Coords to = { 0, 0 };

	int toRide = 0;
	int minutesLeft = 0;
	int minutesStart = 0;
	int fastPass = 0;
};


class Visitor
{
	//int Id;

	void SetDefaults();
	
public:

	std::vector<MinuteDataVI> Statistics;

	std::vector<QWaitTypes::FastPass> FastPassTickets;

	bool StayingOnSite; // only used for fastpass+ mode

	std::vector<int> RideList; // list of every ride this visitor has ridden today

	int Type;

	std::string Name;

	QWaitTypes::Time ArrivalTime;
	QWaitTypes::Time DepartureTime;

	QWaitTypes::Coords Location;

	int StayDuration;

	int ParkStatus;		  //

	TimeStats TimeSpent;

	RideStats Rides;

	int WaitingTime;      // minutes to wait
		
	Travel Travelling;
	
	int LastThreeRides[3];

	Visitor(TypeConfiguration);

	void AddFastPassTicket(int, int, QWaitTypes::Time);
	void SortFastPassTickets();

	void SetNewRide(int ride, int fastpass_ticket, int minutes_left, int distance, QWaitTypes::Coords location);

	void SetRiding(int, int, bool);

	void SetQueuing(float);
	void SetQueuingFastPass(float);

	void SetTravellingMinutesLeft(int);

	void SetWaiting(int);

	std::string GetLocationByMinute(int);
	void SaveMinuteStats();

	void UpdateTravellingLocation(void);

	void UpdateLocation(int, int);

	void UpdateLastRidesList();
};