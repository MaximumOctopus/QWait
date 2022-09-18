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

#include <string>

#include "Constants.h"
#include "Eatery.h"


Eatery::Eatery(std::wstring name, int throughput, int x, int y, bool food, bool drink, int wait_time, int average_adult, int average_child)
{
	Name = name;
	Food = food;
	Drink = drink;

	AveragePriceAdult = average_adult;
	AveragePriceChild = average_child;

	Throughput = throughput;

	X = x;
	Y = y;

	WaitTime = wait_time;
}


void Eatery::AddToQueue(int group)
{
	Queue.push_back(group);

	if (Queue.size() > Statistics.MaxQueue)
	{
		Statistics.MaxQueue = Queue.size();
	}
}


int Eatery::RemoveFromQueue()
{
	if (Queue.size() != 0)
	{
		int Group = Queue[0];

		Queue.erase(Queue.begin());

		return Group;
	}

	return Constants::kNotValidGroup;
}


int Eatery::GetWaitTime(GroupType group_type)
{
	switch (group_type)
	{
	case GroupType::Family:
	{
		int delta = static_cast<int>((double)WaitTime * 1.4);

		return WaitTime + delta;
	}
	case GroupType::AdultCouple:
		return WaitTime;
	case GroupType::AdultGroup:
	{
		int delta = static_cast<int>((double)WaitTime * 1.1);

		return WaitTime + delta;
	}
	case GroupType::Single:
	{
		int delta = static_cast<int>((double)WaitTime * 0.9);

		return WaitTime + delta;
	}
	}
	
	return WaitTime;
}

double Eatery::QueueWaitTime()
{
	return (double)(Queue.size() / Throughput);
}