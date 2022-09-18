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


#include <string>
#include <vector>

#include "Group.h"


struct DayStatistics
{
	int MaxQueue = 0;		// largest queue today
	int Customers = 0;		// custoemrs served today
	int Spend = 0;			// total spend by customers today
};


class Eatery
{
	int WaitTime = 0;

public:

	std::vector<int> Queue;

	DayStatistics Statistics;

	std::wstring Name = L"";

	bool Food = false;
	bool Drink = false;

	int AveragePriceAdult = 0;
	int AveragePriceChild = 0;

	int X = 0;
	int Y = 0;

	int Throughput = 0;

	Eatery(std::wstring, int, int, int, bool, bool, int, int, int);

	void AddToQueue(int);
	int RemoveFromQueue();

	int GetWaitTime(GroupType);

	double QueueWaitTime();
};