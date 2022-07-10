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


#include "Constants.h"
#include "Visitor.h"


Visitor::Visitor(NewVisitorConfiguration type_configuration)
{
	Configuration.Type = type_configuration.type;
	Configuration.TypeInt = QWaitTypes::VisitorTypeToInt(type_configuration.type);

	Configuration.Age = type_configuration.age;

	Rides.maxWaitTime = type_configuration.maxWaitTime;
	Rides.preference  = type_configuration.preference;

	SetDefaults();
}


void Visitor::SetDefaults()
{
	ParkStatus = VisitorParkStatus::OnWay;

	Configuration.Location.x = -1;
	Configuration.Location.y = -1;

	WaitingTime = 0;

	Rides.noRideAvailable = 0;
	Rides.rideShutdown = 0;
	Rides.waitTimeTooLong = 0;
	Rides.noFastPassRideForMe = 0;

	Rides.count = 0;
	Rides.fastPassRides = 0;

	Rides.shortestQueue = 9999.0f;
	Rides.longestQueue = 0.0f;

	Rides.shortestQueueFastPass = 9999;
	Rides.longestQueueFastPass = 0;

	TimeSpent.idle = 0;
	TimeSpent.riding = 0;

	Rides.distanceTravelled = 0;

	TimeSpent.travelling = 0;
	TimeSpent.queuing = 0;
}


void Visitor::SetQueuingLengthStat(float queue_length)
{
	if (queue_length > Rides.longestQueue)
	{
		Rides.longestQueue = queue_length;
	}

	if (queue_length < Rides.shortestQueue)
	{
		Rides.shortestQueue = queue_length;
	}

	ParkStatus = VisitorParkStatus::Queuing;
}


void Visitor::SetQueuingFastPassLengthStat(float queue_length)
{
	if (queue_length > Rides.longestQueueFastPass)
	{
		Rides.longestQueueFastPass = queue_length;
	}

	if (queue_length < Rides.shortestQueueFastPass)
	{
		Rides.shortestQueueFastPass = queue_length;
	}

	ParkStatus = VisitorParkStatus::QueuingFastPass;
}


// =================================================================================================================
// == Purchases ====================================================================================================
// =================================================================================================================


void Visitor::BuyItem(int cost)
{
	Configuration.MoneySpent += cost;
}