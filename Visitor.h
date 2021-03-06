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


#include <vector>

#include "Constants.h"
#include "QWaitTypes.h"


enum class AgeGroup { Baby = 0, Child = 1, Adult = 2 };

enum class VisitorParkStatus {
	OnWay = 0, AtEntrance = 1, Idle = 2, Riding = 3, Queuing = 4, QueuingFastPass = 5,
	Travelling = 6, Waiting = 7, WaitingForOthersInParty = 8, Exited = 9
};


struct NewVisitorConfiguration {
	std::string name = "";

	AgeGroup age = AgeGroup::Adult;

	VisitorType type = VisitorType::Enthusiast;

	float maxWaitTime = 0.0f;
	float preference = 0.0f;
};


struct TimeStats {
	int idle = 0;
	int riding = 0;
	int travelling = 0;
	int queuing = 0;
	int waiting = 0;
};


struct RideStats {
	float maxWaitTime = 0.0f;    // max wait time for a ride (minutes)
	float preference = 0.0f;     // scale from 0 = rides only, 1 = attractions only (actually high priority to low)

	int count = 0;

	int noRideAvailable = 0;
	int waitTimeTooLong = 0;
	int rideShutdown = 0;
	int distanceTravelled = 0; // metres
	int noFastPassRideForMe = 0;
	int nooneCanRideInGroup = 0;
	int rideNotSuitableForMe = 0;

	float shortestQueue = 0.0f;
	float longestQueue = 0.0f;

	float shortestQueueFastPass = 0.0f;
	float longestQueueFastPass = 0.0f;

	int fastPassRides = 0;

	int currentRide = Constants::kNoSelectedRide;
};


struct VisitorConfiguration {

	AgeGroup Age = AgeGroup::Baby;

	VisitorType Type = VisitorType::Enthusiast;
	int TypeInt = 0;

	int MoneySpent = 0;	

	std::string Name;

	QWaitTypes::Coords Location;

	int StayDuration;

	int GetAgeAsInt()
	{
		switch (Age)
		{
		case AgeGroup::Baby:
			return 0;
		case AgeGroup::Child:
			return 1;
		case AgeGroup::Adult:
			return 2;
		}

		return 2;
	}

	std::string GetAgeAsString()
	{
		switch (Age)
		{
		case AgeGroup::Baby:
			return "Baby";
		case AgeGroup::Child:
			return "Child";
		case AgeGroup::Adult:
			return "Adult";
		}

		return "Unknown";
	}
};


class Visitor
{
	void SetDefaults();
	
public:

	VisitorConfiguration Configuration;

	std::vector<int> RideList;		// list of every ride this visitor has ridden today

	VisitorParkStatus ParkStatus;	//

	TimeStats TimeSpent;

	RideStats Rides;

	int WaitingTime;				// minutes to wait
		
	Visitor(NewVisitorConfiguration);

	void SetQueuingLengthStat(float);
	void SetQueuingFastPassLengthStat(float);
	
	void BuyItem(int cost);
};