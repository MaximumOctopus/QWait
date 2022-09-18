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

static const double kFamilyChildrenCount[4] = { 20.0, 70.0, 95.0, 99.0 };
static const double kAdultGroupCount[4]     = { 10.0, 25.0, 40.0, 75.0 };

static const int kBasePostRideWaitTime = 1;

static const int kFamilyConsumptionThreshold = 1000;
static const int kAdultCoupleConsumptionThreshold = 1000;
static const int kAdultGroupConsumptionThreshold = 1000;
static const int kSingleConsumptionThreshold = 1000;

static const double kVisitorTypeCoeff[7][6] = { { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// generic test
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// Animal Kingdom
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// Epcot
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// Hollywood Studios
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// Magic Kingdom
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 },	// Arcade
											    { 0.06, 0.11, 0.40, 0.45, 0.75, 0.98 } };	// Alton Towers

											  // Family AdultCouple AdultGroup Single 
static const double kGroupTypeCoeff[7][3] =   { { 0.50, 0.75, 0.990 },	// generic test
										  	    { 0.50, 0.75, 0.990 },	// Animal Kingdom
											    { 0.50, 0.75, 0.990 },	// Epcot
											    { 0.50, 0.75, 0.990 },	// Hollywood Studios
											    { 0.50, 0.75, 0.990 },	// Magic Kingdom
											    { 0.50, 0.75, 0.990 },	// Arcade
											    { 0.40, 0.75, 0.990 } };	// Alton Towers

enum class GroupParkStatus { OnWay = 0, AtEntrance = 1, Idle = 2, Riding = 3, Queuing = 4, 
	                         QueuingFastPass = 5, QueuingFood = 6, TravellingRide = 7, Waiting = 8, TravellingFood = 9, Eating = 10, Exited = 11 };

enum class GroupVisitorStat {
	NoRideAvailable = 0, RideShutdown = 1, WaitTimeTooLong = 2, NoFastPassRideForMe = 3, TimeSpentRiding = 4, TimeSpentIdle = 5, TimeSpentQueuing = 6,
	TimeSpentQueuingFood = 7, TimeSpentTravelling = 8, TimeSpentTravellingFood = 9, TimeSpentWaiting = 10, TimeSpentEating = 11, NooneCanRideInGroup = 12,
	EateryQueueTooLong = 13
};


struct GroupTravel {
	QWaitTypes::Coords from = { 0, 0 };
	QWaitTypes::Coords to = { 0, 0 };

	int toRide = 0;
	int toEatery = 0;
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

	std::wstring GetTypeToString()
	{
		switch (type)
		{
		case GroupType::Family:
			return L"Family";
			break;

		case GroupType::AdultCouple:
			return L"Adult Couple";
			break;

		case GroupType::AdultGroup:
			return L"Adult Group";
			break;

		case GroupType::Single:
			return L"Single";
			break;
		}

		return L"Unknown!";
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


struct Consumption
{
	double Food = 0;
	double Drink = 0;

	int TimeSinceFood = 0;
	int TimeSinceDrink = 0;

	double Threshold = 1000;
};


struct ParkBehaviour 
{
	Consumption consumption;

	int currentRide = Constants::kNoSelectedRide;
	int currentEatery = Constants::kNoSelectedEatery;

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
	void Demographics();

	void SetGroupSettings();

	void BuildFamily(double);
	void BuildAdultCouple(double);
	void BuildAdultGroup(double);
	void BuildSingle(double);

	VisitorType GetType(double);

	NewVisitorConfiguration GetVisitorGenericAdult();
	NewVisitorConfiguration GetVisitorGenericChild();

	NewVisitorConfiguration GetBaseVisitorConfiguration(VisitorType);

	int GetPostRidingWaitTime(RideExitType, bool);

public:

	std::vector<int> RideList;		// list of every ride this group has ridden today
	std::vector<int> EateryList;		// list of every eatery this group has eaten at today
		
	std::vector<QWaitTypes::FastPass> FastPassTickets;

	GroupStatistics Statistics;

	ParkBehaviour Behaviour;

	std::vector<Visitor> Visitors;

	int AdultCount = 0;
	int ChildCount = 0;

	GroupConfiguration Configuration;

	GroupRiding Riding;
	
	Group(GroupType, int);
	Group(int, int, int, bool, int, int, int, int);

	bool CreateVisitorFromFileData(const std::wstring);

	int GroupRemovedFromRide(int, RideExitType);
	int GroupRemovedFromRideShutdown(RideExitType);

	int GetGroupLeaderType();

	void SetQueuing(int, double);
	void SetQueuingFastPass(int, double);

	void SetRiding(QWaitTypes::Riders, int, int, bool);
	void SetNewRide(int, int, int, int, QWaitTypes::Coords);
	void SetTravellingMinutesLeft(int, bool);
	void SetAtRideQueuing(int);
	void SetAtRideQueuingFastPass(int);

	bool IsRideSuitable(int, bool, bool);
	bool IsFirstTimeRiding(int);

	void UpdateLastRidesList();

	void SetStatusForAllVisitors(GroupParkStatus, VisitorParkStatus);

	int GetMaximumWaitingTime();
	int GetMaximumWaitingTimeFood();

	void UpdateLocation(int, int);
	void UpdateTravellingLocation(void);

	void SetStatForAllVisitors(GroupVisitorStat);

	void SaveMinuteStats();

	std::wstring GetLocationByMinute(int);

	void SetWaiting(int);

	void AddFastPassTicket(int, int, QWaitTypes::Time);
	void SortFastPassTickets();

	std::wstring GetVisitorAgesString();

	void SetNewEatery(int, int, int, QWaitTypes::Coords);

	void ResetFoodDrink();
	void SetAtEateryQueuing(int);
	void UpdateConsumption(int);
};