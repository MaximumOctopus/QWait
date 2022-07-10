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

#include "QWaitTypes.h"
#include "Visitor.h"


enum class GroupType { Family = 0, AdultCouple = 1, AdultGroup = 2, Single = 3 };

static const float kFamilyChildrenCount[4] = { 20.0f, 70.0f, 95.0f, 99.0f };
static const float kAdultGroupCount[4]     = { 10.0f, 25.0f, 40.0f, 75.0f };

static const int kBasePostRideWaitTime = 1;

static const float kVisitorTypeCoeff[7][6] = { { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// generic test
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// Animal Kingdom
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// Epcot
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// Hollywood Studios
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// Magic Kingdom
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f },	// Arcade
											   { 0.06f, 0.11f, 0.40f, 0.45f, 0.75f, 0.98f } };	// Alton Towers

											  // Family AdultCouple AdultGroup Single 
static const float kGroupTypeCoeff[7][3] =   { { 0.50f, 0.75f, 0.990f },	// generic test
											   { 0.50f, 0.75f, 0.990f },	// Animal Kingdom
											   { 0.50f, 0.75f, 0.990f },	// Epcot
											   { 0.50f, 0.75f, 0.990f },	// Hollywood Studios
											   { 0.50f, 0.75f, 0.990f },	// Magic Kingdom
											   { 0.50f, 0.75f, 0.990f },	// Arcade
											   { 0.40f, 0.75f, 0.990f } };	// Alton Towers

enum class GroupParkStatus { OnWay = 0, AtEntrance = 1, Idle = 2, Riding = 3, Queuing = 4, 
	                         QueuingFastPass = 5, Travelling = 6, Waiting = 7, Exited = 8};

enum class GroupVisitorStat {
	NoRideAvailable = 0, RideShutdown = 1, WaitTimeTooLong = 2, NoFastPassRideForMe = 3, TimeSpentRiding = 4,
	TimeSpentIdle = 5, TimeSpentQueuing = 6, TimeSpentTravelling = 7, TimeSpentWaiting = 8, NooneCanRideInGroup = 9
};


struct GroupTravel {
	QWaitTypes::Coords from = { 0, 0 };
	QWaitTypes::Coords to = { 0, 0 };

	int toRide = 0;
	int minutesLeft = 0;
	int minutesStart = 0;
	int fastPass = 0;
};


struct GroupMinuteDataVI {
	QWaitTypes::Coords position = { 0, 0 };

	GroupParkStatus parkStatus = GroupParkStatus::OnWay;
	int parkStatusInt = 0;
};


struct GroupConfiguration {
	GroupType type;

	int templateID = 0;

	int stayDuration = 0; // hours

	bool stayingOnSite = false;

	QWaitTypes::Time arrivalTime = { 0, 0 };
	QWaitTypes::Time departureTime = { 0, 0 };

	std::string GetTypeToString()
	{
		switch (type)
		{
		case GroupType::Family:
			return "Family";
			break;

		case GroupType::AdultCouple:
			return "Adult Couple";
			break;

		case GroupType::AdultGroup:
			return "Adult Group";
			break;

		case GroupType::Single:
			return "Single";
			break;
		}

		return "Unknown!";
	}


	int GetTypeToInt()
	{
		switch (type)
		{
		case GroupType::Family:
			return 0;
			break;

		case GroupType::AdultCouple:
			return 1;
			break;

		case GroupType::AdultGroup:
			return 2;
			break;

		case GroupType::Single:
			return 3;
			break;
		}

		return 0;
	}
};


struct GroupStatistics {
	std::vector<GroupMinuteDataVI> minuteLog;

	TimeStats timeSpent;

	int lastThreeRides[3] = { 0, 0, 0 };
};


struct ParkBehaviour {
	
	int currentRide = Constants::kNoSelectedRide;

	GroupParkStatus parkStatus = GroupParkStatus::OnWay;

	int waitingTime = 0;      // minutes to wait

	GroupTravel travelling;

	QWaitTypes::Coords location;

	int maximumRideWaitingTime = 0;
};


struct GroupRiding
{
	int timeLeft = 0;     // minutes left on the current ride
};


class Group
{
	void SetGroupSettings();

	void BuildFamily(float);
	void BuildAdultCouple(float);
	void BuildAdultGroup(float);
	void BuildSingle(float);

	VisitorType GetType(float);

	NewVisitorConfiguration GetVisitorGenericAdult();
	NewVisitorConfiguration GetVisitorGenericChild();

	NewVisitorConfiguration GetBaseVisitorConfiguration(VisitorType);

	int GetPostRidingWaitTime(RideExitType, bool);

public:

	std::vector<int> RideList;		// list of every ride this group has ridden today
		
	std::vector<QWaitTypes::FastPass> FastPassTickets;

	GroupStatistics Statistics;

	ParkBehaviour Behaviour;

	std::vector<Visitor> Visitors;

	GroupConfiguration Configuration;

	GroupRiding Riding;
	
	Group(GroupType, int);
	Group(int, int, int, bool, int, int, int, int);

	bool CreateVisitorFromFileData(const std::string);

	int GroupRemovedFromRide(int, RideExitType);
	int GroupRemovedFromRideShutdown(RideExitType);

	int GetGroupLeaderType();

	void SetQueuing(int, float);
	void SetQueuingFastPass(int, float);

	void SetRiding(QWaitTypes::Riders, int, int, bool);
	void SetNewRide(int, int, int, int, QWaitTypes::Coords);
	void SetTravellingMinutesLeft(int);
	void SetAtRideQueuing(int);
	void SetAtRideQueuingFastPass(int);

	bool IsRideSuitable(int, bool, bool);
	bool IsFirstTimeRiding(int);

	void UpdateLastRidesList();

	void SetStatusForAllVisitors(GroupParkStatus, VisitorParkStatus);

	int GetMaximumWaitingTime();

	void UpdateLocation(int, int);
	void UpdateTravellingLocation(void);

	void SetStatForAllVisitors(GroupVisitorStat);

	void SaveMinuteStats();

	std::string GetLocationByMinute(int);

	void SetWaiting(int);

	void AddFastPassTicket(int, int, QWaitTypes::Time);
	void SortFastPassTickets();

	std::string GetVisitorAgesString();
};