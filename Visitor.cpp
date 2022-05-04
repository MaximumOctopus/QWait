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


#include <algorithm>
#include "Constants.h"
#include "Visitor.h"


// results in a list where Rides[0] is the high popularity
bool sortByTime(const QWaitTypes::FastPass& r1, const QWaitTypes::FastPass& r2)
{
	return r1.ticket.hours < r2.ticket.hours;
}


Visitor::Visitor(TypeConfiguration typeconfiguration)
{
	Type = typeconfiguration.Type;

	Rides.maxWaitTime = typeconfiguration.maxWaitTime;
	Rides.preference = typeconfiguration.preference;
	StayDuration = typeconfiguration.stayDuration;

	ArrivalTime = typeconfiguration.arrivalTime;
	DepartureTime = typeconfiguration.departureTime;

	StayingOnSite = typeconfiguration.stayingOnSite;

	SetDefaults();
}


void Visitor::SetDefaults()
{
	ParkStatus = Constants::ParkStatusOnWay;

	Location.x = -1;
	Location.y = -1;

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

	Rides.current = kNoCurrentRide;

	TimeSpent.idle = 0;
	TimeSpent.riding = 0;

	Travelling.toRide = kNoDestinationRide;
	Travelling.minutesLeft = 0;
	Rides.distanceTravelled = 0;

	TimeSpent.travelling = 0;
	TimeSpent.queuing = 0;

	LastThreeRides[0] = kNoCurrentRide;
	LastThreeRides[1] = kNoCurrentRide;
	LastThreeRides[2] = kNoCurrentRide;
}


void Visitor::AddFastPassTicket(int type, int ride, QWaitTypes::Time entry_time)
{
	QWaitTypes::FastPass fp = { type, ride, entry_time };

	FastPassTickets.push_back(fp);
}


// order fastpass tickets so earliest are first
void Visitor::SortFastPassTickets()
{
	std::sort(FastPassTickets.begin(), FastPassTickets.end(), sortByTime);
}


void Visitor::SetNewRide(int ride, int fastpass_ticket, int minutes_left, int distance, QWaitTypes::Coords location)
{
	Travelling.toRide = ride;
	Travelling.fastPass = fastpass_ticket;

	SetTravellingMinutesLeft(minutes_left);

	Rides.distanceTravelled += distance;

	Travelling.to.x = location.x;
	Travelling.to.y = location.y;
}


void Visitor::SetRiding(int rideID, int ride_duration, bool from_fast_pass)
{
	Rides.current  = rideID;

	Rides.timeLeft = ride_duration;

	Rides.count++;

	RideList.push_back(rideID);

	if (from_fast_pass)
	{
		FastPassTickets.erase(FastPassTickets.begin());

		Rides.fastPassRides++;
	}

	ParkStatus = Constants::ParkStatusRiding;
}


void Visitor::SetQueuing(float queue_length)
{
	if (queue_length > Rides.longestQueue)
	{
		Rides.longestQueue = queue_length;
	}

	if (queue_length < Rides.shortestQueue)
	{
		Rides.shortestQueue = queue_length;
	}
	
	ParkStatus = Constants::ParkStatusQueuing;
}


void Visitor::SetQueuingFastPass(float queue_length)
{
	if (queue_length > Rides.longestQueueFastPass)
	{
		Rides.longestQueueFastPass = queue_length;
	}

	if (queue_length < Rides.shortestQueueFastPass)
	{
		Rides.shortestQueueFastPass = queue_length;
	}

	ParkStatus = Constants::ParkStatusQueuingFastPass;
}


void Visitor::SetTravellingMinutesLeft(int minutes)
{
	switch (Type)
	{
		case Constants::VisitorTypeEnthusiast:
			Travelling.minutesLeft = minutes - 1;

			break;

		case Constants::VisitorTypeFan:
			Travelling.minutesLeft = minutes - 1;

			break;

		case Constants::VisitorTypeTourist:
			Travelling.minutesLeft = minutes;

			break;

		case Constants::VisitorTypeActivity:
			Travelling.minutesLeft = minutes;

			break;

		case Constants::VisitorTypePassholder:
			Travelling.minutesLeft = minutes;

			break;

		case Constants::VisitorTypeEPassHolder:
			Travelling.minutesLeft = minutes;

			break;

		case Constants::VisitorTypeTikToker:
			Travelling.minutesLeft = minutes - 1;

			break;
	}

	Travelling.from.x = Location.x;
	Travelling.from.y = Location.y;

	if (Travelling.minutesLeft <= 0)
	{
		Travelling.minutesLeft = 1;
	}

	Travelling.minutesStart = Travelling.minutesLeft;

	TimeSpent.travelling += Travelling.minutesLeft;

	ParkStatus = Constants::ParkStatusTravelling;
}


void Visitor::SetWaiting(int time)
{
	WaitingTime = time;
	ParkStatus = Constants::ParkStatusWaiting;
}


void Visitor::UpdateTravellingLocation(void)
{
	float dx = abs((float)Travelling.from.x - (float)Travelling.to.x);
	float dy = abs((float)Travelling.from.y - (float)Travelling.to.y);

	float d = sqrtf(dx * dx + dy * dy);

	float deltad = d * (((float)Travelling.minutesStart - (float)Travelling.minutesLeft) / (float)Travelling.minutesStart);
	
	int newx = static_cast<int>((deltad / d) * ((float)Travelling.to.x - (float)Travelling.from.x) + (float)Travelling.from.x);
	int newy = static_cast<int>((deltad / d) * ((float)Travelling.to.y - (float)Travelling.from.y) + (float)Travelling.from.y);
	
	Location.x = newx;
	Location.y = newy;
}


// update location when travelling by interpolating direct line from x,y -> x1,y1
void Visitor::UpdateLocation(int x, int y)
{
	Location.x = x;
	Location.y = y;

	if (x == kLocationExitedPark && y == kLocationExitedPark)
	{
		ParkStatus = Constants::ParkStatusExited;
	}
}


void Visitor::UpdateLastRidesList()
{
	LastThreeRides[0] = LastThreeRides[1];
	LastThreeRides[1] = LastThreeRides[2];
	LastThreeRides[2] = Rides.current;
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


void Visitor::SaveMinuteStats()
{
	MinuteDataVI m;

	m.position.x = Location.x;
	m.position.y = Location.y;

	m.parkStatus = ParkStatus;

	Statistics.push_back(m);
}


std::string Visitor::GetLocationByMinute(int minute)
{
	return "," + std::to_string(minute) + "," + std::to_string(Statistics[minute].position.x) + "," + std::to_string(Statistics[minute].position.y) + "," + std::to_string(Statistics[minute].parkStatus);
}