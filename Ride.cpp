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


#include <iostream>

#include "Ride.h"
#include "Utility.h"


Ride::Ride(RideType ride_type, std::wstring name, int ride_length, RideExitType ride_exit_type, int hourly_throughput, int popularity, int x, int y, int is_fast_pass, int fp_reserve_percent,
	QWaitTypes::Time open, QWaitTypes::Time close, int open_hours,
	bool AdultValid, bool ChildValid)
{
	RideOperation.isShutdown = false;

	RideOperation.rideType   = ride_type;
	RideOperation.rideExitType = ride_exit_type;

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

	RideOperation.AdultValid = AdultValid;
	RideOperation.ChildValid = ChildValid;

	CurrentRiders = 0;

	DailyStatistics.maxQueueLength = 0;

	FastPass.mode = is_fast_pass;
	FastPass.percentage = fp_reserve_percent;

	ConfigureThroughput(hourly_throughput);

	InitFastPass();

	for (int t = 0; t < kClosestCacheSize; t++)
	{
		ClosestCache[t] = Constants::kNoSelectedRide;
	}
}


void Ride::Close()
{
	RideOperation.isOpen = false;
	RideOperation.isShutdown = true;

	UpdateDailyStatistics();
}


// returns a unique reference for this ride's characteristics
std::wstring Ride::GetUniqueReference()
{
	std::wstring data = L"";
	
	switch (RideOperation.rideType)
	{
		case RideType::Continuous:
			data = RideOperation.name + std::to_wstring(RideOperation.rideLength) + std::to_wstring(RideOperation.position.x) + std::to_wstring(RideOperation.position.y) + std::to_wstring(RideOperation.Popularity) +
				Utility::FormatTime(RideOperation.open) + Utility::FormatTime(RideOperation.close) + 
				std::to_wstring(FastPass.mode) + std::to_wstring(FastPass.percentage) +
				std::to_wstring(RideThroughput.totalPerHour);
			break;
		
		case RideType::Show:
			data = RideOperation.name + std::to_wstring(RideOperation.rideLength) + std::to_wstring(RideOperation.position.x) + std::to_wstring(RideOperation.position.y) + std::to_wstring(RideOperation.Popularity) +
				Utility::FormatTime(RideOperation.open) + Utility::FormatTime(RideOperation.close) + std::to_wstring(RideOperation.ShowStartTime) +
				std::to_wstring(FastPass.mode) + std::to_wstring(FastPass.percentage) +
				std::to_wstring(RideThroughput.totalPerHour) + std::to_wstring(RideThroughput.showCapacity);
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
	double percentage = (double)FastPass.percentage / 100.0f;

	return static_cast<int>((double)RideThroughput.totalPerHour * percentage);
}


void Ride::ConfigureThroughput(int hourly_throughput)
{
	RideThroughput.totalPerHour = hourly_throughput;

	if (FastPass.mode != 0)
	{
		RideThroughput.perMinuteTotal = (double)hourly_throughput / 60.0f;

		RideThroughput.perMinute = ((double)hourly_throughput * (Constants::StandardHourlyPercent / 100.0f)) / 60.0f;
		RideThroughput.perMinuteI = static_cast<int>(std::lround(RideThroughput.perMinute));

		RideThroughput.perMinuteFastPass = ((double)hourly_throughput * (Constants::FastPassHourlyPercent / 100.0f)) / 60.0f;
		RideThroughput.perMinuteIFastPass = static_cast<int>(std::lround(RideThroughput.perMinuteFastPass));
	}
	else
	{
		RideThroughput.perMinuteTotal = (double)hourly_throughput / 60.0f;

		RideThroughput.perMinute = (double)hourly_throughput / 60.0f;
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
		RideThroughput.showCapacityFastPass = static_cast<int>((double)show_capacity * ((double)fastpass_percent / 100.0f));
	}
	else
	{
		RideThroughput.showCapacityFastPass = 0;
	}
}


void Ride::AddToQueue(QWaitTypes::Riders riders)
{
	Queue.push_back(riders);
}


QWaitTypes::Riders Ride::RemoveFromQueue()
{
	if (Queue.size() != 0)
	{
		QWaitTypes::Riders Visitor = Queue[0];

		Queue.erase(Queue.begin());

		return Visitor;
	}

	return { Constants::kNotValidGroup, {} };
}


QWaitTypes::Riders Ride::NextItemInQueue()
{
	if (Queue.size() != 0)
	{
		QWaitTypes::Riders Visitor = Queue[0];

		return Visitor;
	}

	return { Constants::kNotValidGroup, {} };
}


int Ride::QueueSize()
{
	auto count = 0;

	for (int q = 0; q < Queue.size(); q++)
	{
		count += Queue[q].memberID.size();
	}

	return count;
}


void Ride::AddToQueueFastPass(QWaitTypes::Riders riders)
{
	QueueFastPass.push_back(riders);
}


QWaitTypes::Riders Ride::RemoveFromQueueFastPass()
{
	if (QueueFastPass.size() != 0)
	{
		QWaitTypes::Riders Visitor = QueueFastPass[0];

		QueueFastPass.erase(QueueFastPass.begin());

		return Visitor;
	}

	return { Constants::kNotValidGroup, {} };
}


QWaitTypes::Riders Ride::NextItemInQueueFastPass()
{
	if (QueueFastPass.size() != 0)
	{
		QWaitTypes::Riders Visitor = QueueFastPass[0];

		return Visitor;
	}

	return { Constants::kNotValidGroup, {} };
}


int Ride::QueueSizeFastPass()
{
	auto count = 0;

	for (int q = 0; q < QueueFastPass.size(); q++)
	{
		count += QueueFastPass[q].memberID.size();
	}

	return count;
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
double Ride::WaitTime(int is_fast_pass)
{
	if (is_fast_pass != kNoSelectedFastPassTicket)
	{
		return (double)(QueueSizeFastPass() / RideThroughput.perMinuteFastPass);
	}
	else
	{
		return ((double)QueueSize() + (double)QueueSizeFastPass()) / RideThroughput.perMinuteTotal;
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

	m.queueSize = QueueSize();
	m.queueSizeFastPass = QueueSizeFastPass();
	m.riders = CurrentRiders;
	m.waitTimeMinutes = static_cast<int>(WaitTime(kNoSelectedFastPassTicket));
	m.waitTimeMinutesFastPass = static_cast<int>(WaitTime(0));

	Statistics.push_back(m);

	UpdateDailyStatistics();
}


void Ride::UpdateDailyStatistics()
{
	int queue = QueueSize();

	if (queue > DailyStatistics.maxQueueLength)
	{
		DailyStatistics.maxQueueLength = queue;
	}
}


std::wstring Ride::GetMinuteStatFor(int minute)
{
	return std::to_wstring(Statistics[minute].queueSize) + L"," +
	       std::to_wstring(Statistics[minute].riders) + L"," +
		   std::to_wstring(Statistics[minute].waitTimeMinutes);
}