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


#include <iostream>
#include "Ride.h"
#include "Utility.h"


Ride::Ride(int Type, std::string name, int ride_length, int hourly_throughput, int popularity, int x, int y, int is_fast_pass, int fp_reserve_percent, QWaitTypes::Time open, QWaitTypes::Time close, int open_hours)
{
	RideOperation.isShutdown = false;

	RideOperation.rideType   = Type;

	RideOperation.name       = name;

	RideOperation.rideLength = ride_length;

	RideOperation.position.x = x;
	RideOperation.position.y = y;

	RideOperation.isOpen     = true;

	RideOperation.open = open;
	RideOperation.close = close;
	RideOperation.operationHours = open_hours;

	// this allows for the extra hour the park is open; rides are closed, but queues will be emptied.
	RideOperation.operationHours++;

	RideOperation.Popularity = popularity;

	RideOperation.ShowStartTime = 0;

	CurrentRiders = 0;

	DailyStatistics.maxQueueLength = 0;

	FastPass.mode = is_fast_pass;
	FastPass.percentage = fp_reserve_percent;

	ConfigureThroughput(hourly_throughput);

	InitFastPass();

	for (int t = 0; t < kClosestCacheSize; t++)
	{
		ClosestCache[t] = kNoSelectedRide;
	}
}


void Ride::Close()
{
	RideOperation.isOpen = false;
	RideOperation.isShutdown = true;

	UpdateDailyStatistics();
}


// returns a unique reference for this ride's characteristics
std::string Ride::GetUniqueReference()
{
	std::string data = "";
	
	switch (RideOperation.rideType)
	{
		case kRideTypeContinuous:
			data = RideOperation.name + std::to_string(RideOperation.rideLength) + std::to_string(RideOperation.position.x) + std::to_string(RideOperation.position.y) + std::to_string(RideOperation.Popularity) +
				Utility::FormatTime(RideOperation.open) + Utility::FormatTime(RideOperation.close) + 
				std::to_string(FastPass.mode) + std::to_string(FastPass.percentage) +
				std::to_string(RideThroughput.totalPerHour);
			break;
		
		case kRideTypeShow:
			data = RideOperation.name + std::to_string(RideOperation.rideLength) + std::to_string(RideOperation.position.x) + std::to_string(RideOperation.position.y) + std::to_string(RideOperation.Popularity) +
				Utility::FormatTime(RideOperation.open) + Utility::FormatTime(RideOperation.close) + std::to_string(RideOperation.ShowStartTime) +
				std::to_string(FastPass.mode) + std::to_string(FastPass.percentage) +
				std::to_string(RideThroughput.totalPerHour) + std::to_string(RideThroughput.showCapacity);
			break;
	}

	return Utility::GetMD5(data);
}


void Ride::InitFastPass()
{
	if (FastPass.mode != 0)
	{
		for (int h = 0; h < 24; h++)
		{
			if (RideOperation.open.hours <= h && RideOperation.close.hours > h)
			{
				FastPass.ticketsLeftFoHour[h] = ResetFastPassTicketsLeft();
			}
			else
			{
				FastPass.ticketsLeftFoHour[h] = 0;
			}
		}
	}
}


// tickets left per hour
int Ride::ResetFastPassTicketsLeft()
{
	float percentage = (float)FastPass.percentage / 100.0f;

	return static_cast<int>((float)RideThroughput.totalPerHour * percentage);
}


void Ride::ConfigureThroughput(int hourly_throughput)
{
	RideThroughput.totalPerHour = hourly_throughput;

	if (FastPass.mode != 0)
	{
		RideThroughput.perMinuteTotal = (float)hourly_throughput / 60.0f;

		RideThroughput.perMinute = ((float)hourly_throughput * (Constants::StandardHourlyPercent / 100.0f)) / 60.0f;
		RideThroughput.perMinuteI = static_cast<int>(std::lround(RideThroughput.perMinute));

		RideThroughput.perMinuteFastPass = ((float)hourly_throughput * (Constants::FastPassHourlyPercent / 100.0f)) / 60.0f;
		RideThroughput.perMinuteIFastPass = static_cast<int>(std::lround(RideThroughput.perMinuteFastPass));
	}
	else
	{
		RideThroughput.perMinuteTotal = (float)hourly_throughput / 60.0f;

		RideThroughput.perMinute = (float)hourly_throughput / 60.0f;
		RideThroughput.perMinuteI = static_cast<int>(std::lround(RideThroughput.perMinute));

		RideThroughput.perMinuteFastPass = 0.0f;
		RideThroughput.perMinuteIFastPass = 0;
	}
}


void Ride::ConfigureShowThroughput(int show_capacity, int fastpass_percent)
{
	RideThroughput.showCapacity = show_capacity;

	if (FastPass.mode != 0)
	{
		RideThroughput.showCapacityFastPass = static_cast<int>((float)show_capacity * ((float)fastpass_percent / 100.0f));
	}
	else
	{
		RideThroughput.showCapacityFastPass = 0;
	}
}


void Ride::AddToQueue(int visitor)
{
	Queue.push_back(visitor);
}


int Ride::RemoveFromQueue()
{
	if (Queue.size() != 0)
	{
		int VisitorID = Queue[0];

		Queue.erase(Queue.begin());

		return VisitorID;
	}

	return kNotValidVisitor;
}


size_t Ride::QueueSize()
{
	return Queue.size();
}


void Ride::AddToQueueFastPass(int visitor)
{
	QueueFastPass.push_back(visitor);
}


int Ride::RemoveFromQueueFastPass()
{
	if (QueueFastPass.size() != 0)
	{
		int Visitor = QueueFastPass[0];

		QueueFastPass.erase(QueueFastPass.begin());

		return Visitor;
	}

	return kNotValidVisitor;
}


size_t Ride::QueueSizeFastPass()
{
	return QueueFastPass.size();
}


// returns 24-hour clock time, valid for 1 hour
int Ride::GetFastPassTicket(QWaitTypes::Time arrive, QWaitTypes::Time leave)
{
	for (int h = arrive.hours; h < leave.hours; h++)
	{
		if (FastPass.ticketsLeftFoHour[h] != 0)
		{
			FastPass.ticketsLeftFoHour[h]--;

			return h;
		}
	}

	return kNoSelectedFastPassTicket;
}


// checks to see which fastpass ticket will be selected next
// currenttime refers to a one hour slot from CurrentTime to CurrentTime + 1 (24-hour clock)
int Ride::ViewFastPassTicket(QWaitTypes::Time arrive, QWaitTypes::Time leave)
{
	for (int h = arrive.hours; h < leave.hours; h++)
	{
		if (FastPass.ticketsLeftFoHour[h] != 0)
		{
			return h;
		}
	}

	return kNoSelectedFastPassTicket;
}


// returns the wait time in minutes
float Ride::WaitTime(int is_fast_pass)
{
	if (is_fast_pass != kNoSelectedFastPassTicket)
	{
		return (float)(QueueFastPass.size() / RideThroughput.perMinuteFastPass);
	}
	else
	{
		return ((float)Queue.size() + (float)QueueFastPass.size()) / RideThroughput.perMinuteTotal;
	}
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


size_t Ride::GetMinuteStatsCount()
{
	return Statistics.size();
}


void Ride::UpdateMinuteStats()
{
	MinuteData m;

	m.queueSize = Queue.size();
	m.queueSizeFastPass = QueueFastPass.size();
	m.riders = CurrentRiders;
	m.waitTimeMinutes = static_cast<int>(WaitTime(kNoSelectedFastPassTicket));
	m.waitTimeMinutesFastPass = static_cast<int>(WaitTime(0));

	Statistics.push_back(m);

	UpdateDailyStatistics();
}


void Ride::UpdateDailyStatistics()
{
	if (Queue.size() > DailyStatistics.maxQueueLength)
	{
		DailyStatistics.maxQueueLength = Queue.size();
	}
}


std::string Ride::GetMinuteStatFor(int minute)
{
	return std::to_string(Statistics[minute].queueSize) + "," +
	       std::to_string(Statistics[minute].riders) + "," +
		   std::to_string(Statistics[minute].waitTimeMinutes);
}