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
#include <random>

#include "Constants.h"
#include "Engine.h"
#include "ParkController.h"
#include "ReportUtility.h"
#include "Utility.h"
#include "VisitorController.h"


extern ParkController* GParkController;
extern VisitorController* GVisitorController;


Engine::Engine(FastPassType fast_pass, int park_open_time, int park_close_time, bool show_output, int update_rate, bool handle_food_drink, int temp)
{
	UpdateRate = update_rate;

	ParkOpen = true;

	ShowOutput = show_output;

	FastPassMode = fast_pass;

	HandleFoodDrink = handle_food_drink;

	GParkController->FastPassMode = FastPassMode;

	CurrentTime.hours = park_open_time;
	CurrentTime.minutes = 0;

	TimeRidesClose = park_close_time;
	TimeParkCloses = park_close_time + 1;

	VisitorsInPark = 0;

	Temperature = temp;
}


void Engine::OutputStatus(const std::wstring status)
{
	if (ShowOutput)
	{
		std::wcout << status << std::endl;
	}
}


void Engine::Run(bool report_mxm, bool report_visitor_location)
{
	if (FastPassMode != FastPassType::None)
	{
		PreArrivalFastPass();
	}

	OutputStatus(L"Park open!                                                " + ReportUtility::BarGraphScale());

	while (CurrentTime.hours != TimeParkCloses)
	{
		// ========================================================================================================
		// == Visitor Processing ==================================================================================
		// ========================================================================================================

		for (int g = 0; g < GVisitorController->Groups.size(); g++)
		{
			switch (GVisitorController->Groups[g].Behaviour.parkStatus)
			{
				// let's see if the visitor should enter the park
			case GroupParkStatus::OnWay:

				if (Utility::IsTimeEqual(CurrentTime, GVisitorController->Groups[g].Configuration.arrivalTime))
				{
					GVisitorController->Groups[g].SetStatusForAllVisitors(GroupParkStatus::AtEntrance, VisitorParkStatus::AtEntrance);

					GVisitorController->Groups[g].UpdateLocation(GParkController->entrance.position.x,
						                                        GParkController->entrance.position.y);
				}

				break;

			case GroupParkStatus::AtEntrance:

				ParkStatusEntrance(g);

				break;

			case GroupParkStatus::Idle:

				ParkStatusIdle(g);

				break;

			case GroupParkStatus::Riding:

				ParkStatusRiding(g);

				break;

			case GroupParkStatus::Queuing:
			case GroupParkStatus::QueuingFastPass:

				GVisitorController->Groups[g].SetStatForAllVisitors(GroupVisitorStat::TimeSpentQueuing);

				break;

			case GroupParkStatus::QueuingFood:

				GVisitorController->Groups[g].SetStatForAllVisitors(GroupVisitorStat::TimeSpentQueuingFood);

				break;

			case GroupParkStatus::TravellingRide:

				ParkStatusTravellingRide(g);

				break;

			case GroupParkStatus::Waiting:

				ParkStatusWaiting(g);

				break;

			case GroupParkStatus::TravellingFood:

				ParkStatusTravellingFood(g);

				break;

			case GroupParkStatus::Eating:

				ParkStatusEating(g);

				break;

			case GroupParkStatus::Exited:
				break;
			}

			if (report_visitor_location)
			{
				GVisitorController->Groups[g].SaveMinuteStats();
			}

			// ========================================================================================================
			// == Food and Drink ======================================================================================
			// ========================================================================================================

			if (HandleFoodDrink)
			{
				GVisitorController->Groups[g].UpdateConsumption(Temperature);

				GVisitorController->Groups[g].Behaviour.consumption.TimeSinceDrink++;
			}
		}

		// ========================================================================================================
		// == Entrance Queue Processing ===========================================================================
		// ========================================================================================================

		if (GParkController->EntranceQueueSize() != 0)
		{
			EntranceProcessQueue();
		}

		// ========================================================================================================
		// == Ride Processing =====================================================================================
		// ========================================================================================================

		for (int r = 0; r < GParkController->Rides.size(); r++)
		{
			if (!GParkController->Rides[r].RideOperation.isShutdown)
			{
				ProcessRide(r);
			}

			if (report_mxm)
			{
				GParkController->Rides[r].UpdateMinuteStats();
			}
		}

		// ========================================================================================================
		// == Food Drink Processing ===============================================================================
		// ========================================================================================================

		if (HandleFoodDrink)
		{
			for (int e = 0; e < GParkController->Eateries.size(); e++)
			{
				EateryProcessQueue(e);
			}
		}

		// ========================================================================================================

		if (report_mxm)
		{
			GVisitorController->UpdateMinuteStats(Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes));
		}

		// ========================================================================================================

		GParkController->Minutes++;

		// ========================================================================================================

		UpdateClock();
	}

	GParkController->Minutes--;

	OutputStatus(L"\nPark closed!");
}


void Engine::UpdateClock()
{
	if (CurrentTime.minutes % UpdateRate == 0)
	{
		ShowLiveStats();
	}

	CurrentTime.minutes++;

	if (CurrentTime.minutes == 60)
	{
		CurrentTime.minutes = 0;
		CurrentTime.hours++;

		if (CurrentTime.hours == TimeRidesClose)
		{
			ParkOpen = false;
		}
	}
}


QWaitTypes::GetRide Engine::GetRide(int group)
{
	int SelectedRide = Constants::kNoSelectedRide;
	size_t CheckingRides = GParkController->Rides.size() * 2;

	// == ride selection "magic" ================================================

	while (CheckingRides != 0)
	{
		int num = rand() % kSelectionChoiceCacheCount;

		SelectedRide = GParkController->SelectionChoiceCache[GVisitorController->Groups[group].GetGroupLeaderType()][num];

		if (GParkController->Rides[SelectedRide].WaitTime(kNoSelectedFastPassTicket) <= GVisitorController->Groups[group].Behaviour.maximumRideWaitingTime)
		{
			int isbetteravailable = IsRideAvailableCloser(group, SelectedRide);

			QWaitTypes::FastPass FastPassDetails = IsFastPassTicketSoon(group, SelectedRide);

			if (isbetteravailable != Constants::kNoSelectedRide && FastPassDetails.ride == Constants::kNoSelectedRide)
			{
				return { isbetteravailable, kNoSelectedFastPassTicket };
			}
			else if (FastPassDetails.ride != Constants::kNoSelectedRide)
			{
				return { FastPassDetails.ride, FastPassDetails.ticket.hours };
			}

			return { SelectedRide, kNoSelectedFastPassTicket };
		}
		else
		{
			//std::wcout << "- " << SelectedRide << " NO \n";
		}

		CheckingRides--;
	}

	// ==========================================================================

	return { Constants::kNoDestinationRide, kNoSelectedFastPassTicket };
}


// if there's a ride which has better popularity, lower wait time, and is closer, then select this instead
int Engine::IsRideAvailableCloser(int group, int possible_destination)
{
	if (GVisitorController->Groups[group].Behaviour.currentRide == Constants::kNoCurrentRide)
	{
		return Constants::kNoSelectedRide;
	}

	int CurrentRide = GVisitorController->Groups[group].Behaviour.currentRide;

	Ride r = GParkController->Rides[CurrentRide];
	Ride d = GParkController->Rides[possible_destination];

	for (int i = 0; i < 5; i++)
	{
		Ride c = GParkController->Rides[r.ClosestCache[i]];

		if (c.RideOperation.Popularity > d.RideOperation.Popularity &&
			GParkController->GetDistanceBetweenInMinutes(r.ClosestCache[i], CurrentRide) < GParkController->GetDistanceBetweenInMinutes(possible_destination, CurrentRide) &&
			c.WaitTime(kNoSelectedFastPassTicket) < d.WaitTime(kNoSelectedFastPassTicket) &&
			r.ClosestCache[i] != GVisitorController->Groups[group].Statistics.lastThreeRides[2] &&
			r.ClosestCache[i] != GVisitorController->Groups[group].Statistics.lastThreeRides[1])
		{
			return r.ClosestCache[i];
		}
	}

	return Constants::kNoSelectedRide;
}


// =================================================================================================================
// == FastPass Processing ==========================================================================================
// =================================================================================================================


QWaitTypes::FastPass Engine::IsFastPassTicketSoon(int group, int possible_destination)
{
	if (FastPassMode != FastPassType::None)
	{
		if (GVisitorController->Groups[group].FastPassTickets.size() != 0)
		{
			int TimeRequired = static_cast<int>(GParkController->Rides[possible_destination].WaitTime(kNoSelectedFastPassTicket)) +
												GParkController->GetDistanceBetweenInMinutes(GVisitorController->Groups[group].Behaviour.currentRide, possible_destination) +
												GParkController->GetDistanceBetweenInMinutes(possible_destination, GVisitorController->Groups[group].FastPassTickets[0].ride);

			int TimeToFPT = Utility::Duration(CurrentTime, GVisitorController->Groups[group].FastPassTickets[0].ticket);

			// fastpass slot is 1 hour
			if (TimeRequired < TimeToFPT + 59)
			{
				// plenty of time to get to this ride
			}
			else
			{
				return GVisitorController->Groups[group].FastPassTickets[0];
			}
		}
	}

	return { Constants::kNoSelectedType, Constants::kNoSelectedRide, { 0, 0 } };
}


void Engine::PreArrivalFastPass()
{
	size_t count = 0;

	if (FastPassMode != FastPassType::PlusNoStayBenefit)
	{
		// Pass 1, visitors who are "staying at the park"

		OutputStatus(L"Processing FastPass tickets...");

		for (int g = 0; g < GVisitorController->Groups.size(); g++)
		{
			if (GVisitorController->Groups[g].Configuration.stayingOnSite)
			{
				ProcessFastPassesForGroup(g);

				count += GVisitorController->Groups[g].FastPassTickets.size();
			}
		}
	}

	GVisitorController->FastPassStats.ticketsEarly = count;

	// Pass 2, everyone else! =========================================================

	for (int g = 0; g < GVisitorController->Groups.size(); g++)
	{
		if (!GVisitorController->Groups[g].Configuration.stayingOnSite || FastPassMode == FastPassType::PlusNoStayBenefit)
		{
			ProcessFastPassesForGroup(g);

			count += GVisitorController->Groups[g].FastPassTickets.size();
		}

		GVisitorController->Groups[g].SortFastPassTickets();
	}

	GVisitorController->FastPassStats.ticketsEarlyAll = count;
}


void Engine::ProcessFastPassesForGroup(int group)
{
	QWaitTypes::FastPass FP1 = GetFastPassRide(1, GVisitorController->Groups[group].Configuration.arrivalTime, GVisitorController->Groups[group].Configuration.departureTime);

	if (FP1.ride != Constants::kNoSelectedRide)
	{
		GVisitorController->Groups[group].AddFastPassTicket(FP1.type, FP1.ride, FP1.ticket);
	}
	else
	{
		GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NoFastPassRideForMe);
	}

	for (int x = 0; x < 2; x++)
	{
		QWaitTypes::FastPass FP2 = GetFastPassRide(2, GVisitorController->Groups[group].Configuration.arrivalTime, GVisitorController->Groups[group].Configuration.departureTime);

		if (FP2.ride != Constants::kNoSelectedRide)
		{
			GVisitorController->Groups[group].AddFastPassTicket(FP2.type, FP2.ride, FP2.ticket);
		}
		else
		{
			GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NoFastPassRideForMe);
		}
	}

	for (int x = 0; x < 3; x++)
	{
		QWaitTypes::FastPass FP3 = GetFastPassRide(3, GVisitorController->Groups[group].Configuration.arrivalTime, GVisitorController->Groups[group].Configuration.departureTime);

		if (FP3.ride != Constants::kNoSelectedRide)
		{
			GVisitorController->Groups[group].AddFastPassTicket(FP3.type, FP3.ride, FP3.ticket);
		}
		else
		{
			GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NoFastPassRideForMe);
		}
	}
}


// guests allowed 1 type 1, 2 from type 2, 3 from type 3 (if available)
QWaitTypes::FastPass Engine::GetFastPassRide(int type, QWaitTypes::Time arrive, QWaitTypes::Time leave)
{
	if (GParkController->FastPassTypes[type - 1].size() != 0)
	{
		int fpt = 0;
		int ticket = kNoSelectedFastPassTicket;

		do
		{
			int ride = GParkController->FastPassTypes[type - 1][fpt];
			
			ticket = GParkController->Rides[ride].ViewFastPassTicket(arrive, leave);

			if (ticket != kNoSelectedFastPassTicket)
			{
				if (Utility::IsTicketValidForTime(arrive, leave, { ticket, 0 }))
				{
					return { type, ride, { GParkController->Rides[ride].GetFastPassTicket(arrive, leave), 0 } };
				}

				ticket = kNoSelectedFastPassTicket;
			}

			fpt++;

		} while (fpt < GParkController->FastPassTypes[type - 1].size() && ticket == kNoSelectedFastPassTicket);
	}

	return { Constants::kNoSelectedType, Constants::kNoSelectedRide, { 0, 0 } };
}


void Engine::GetReplacementFastPassRide(int group)
{
	if (GVisitorController->Groups[group].FastPassTickets.size() == 0)
	{
		if (GParkController->entrance.fastPassType == 1)
		{
			QWaitTypes::FastPass FP1 = GetFastPassRide(1, CurrentTime, GVisitorController->Groups[group].Configuration.departureTime);

			if (FP1.ride != Constants::kNoSelectedRide)
			{
				GVisitorController->Groups[group].AddFastPassTicket(FP1.type, FP1.ride, FP1.ticket);

				return;
			}

			QWaitTypes::FastPass FP2 = GetFastPassRide(2, CurrentTime, GVisitorController->Groups[group].Configuration.departureTime);

			if (FP2.ride != Constants::kNoSelectedRide)
			{
				GVisitorController->Groups[group].AddFastPassTicket(FP2.type, FP2.ride, FP2.ticket);

				return;
			}
		}
		else if (GParkController->entrance.fastPassType == 3)
		{
			QWaitTypes::FastPass FP3 = GetFastPassRide(3, CurrentTime, GVisitorController->Groups[group].Configuration.departureTime);

			if (FP3.ride != Constants::kNoSelectedRide)
			{
				GVisitorController->Groups[group].AddFastPassTicket(FP3.type, FP3.ride, FP3.ticket);

				return;
			}
		}

		GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NoFastPassRideForMe);
	}
}


// =================================================================================================================
// == Ride status processing =======================================================================================
// =================================================================================================================


void Engine::EntranceProcessQueue()
{
	int count = 0;
	int GroupID = Constants::kNotValidGroup;

	do
	{
		GroupID = GParkController->RemoveFromEntranceQueue();

		if (GroupID != Constants::kNotValidGroup)
		{
			GVisitorController->Groups[GroupID].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);

			for (int v = 0; v < GVisitorController->Groups[GroupID].Visitors.size(); v++)
			{
				GVisitorController->Groups[GroupID].Visitors[v].BuyItem(GParkController->BuyTicket(GVisitorController->Groups[GroupID].Visitors[v].Configuration.Age, false));
			}
		}

		count++;

	} while (count < GParkController->entrance.throughPutMinute && GroupID != Constants::kNotValidGroup);
}


void Engine::ProcessRide(int ride)
{
	if (GParkController->Rides[ride].RideOperation.isOpen)
	{
		if (Utility::IsTimeEqual(CurrentTime, GParkController->Rides[ride].RideOperation.close))
		{
			RideEmptyQueue(ride);

			GParkController->Rides[ride].Close();

			return;
		}
	}
	else
	{
		if (Utility::IsTimeEqual(CurrentTime, GParkController->Rides[ride].RideOperation.open))
		{
			GParkController->Rides[ride].RideOperation.isOpen = true;
		}
	}

	switch (GParkController->Rides[ride].RideOperation.rideType)
	{
	case RideType::Continuous:
	{
		if (GParkController->Rides[ride].QueueSize() != 0 || GParkController->Rides[ride].QueueSizeFastPass() != 0)
		{
			RideContinuousProcessQueue(ride);
		}

		break;
	}
	case RideType::Show:
	{
		if ((GParkController->Rides[ride].QueueSize() != 0 || GParkController->Rides[ride].QueueSizeFastPass() != 0) && CurrentTime.minutes % GParkController->Rides[ride].RideOperation.ShowStartTime == 0)
		{
			RideShowProcessQueue(ride);
		}

		break;
	}
	}

	GParkController->Rides[ride].UpdateDailyStatistics();
}


void Engine::RideContinuousProcessQueue(int ride)
{
	int count = 0;
	QWaitTypes::Riders Visitor;

	if (GParkController->Rides[ride].FastPass.mode != 0)
	{
		do
		{
			Visitor = GParkController->Rides[ride].NextItemInQueueFastPass();
			   
			// if this group is larger than the maximum throughput per minute then we have to let them all through
			if (Visitor.group != Constants::kNotValidGroup && (Visitor.memberID.size() <= (GParkController->Rides[ride].RideThroughput.perMinuteIFastPass - count) || Visitor.memberID.size() > GParkController->Rides[ride].RideThroughput.perMinuteIFastPass))
			{			
				GParkController->Rides[ride].RemoveFromQueueFastPass();

				GVisitorController->Groups[Visitor.group].SetRiding(Visitor, ride, GParkController->Rides[ride].RideOperation.rideLength, true);

				GParkController->Rides[ride].DailyStatistics.totalRiders += Visitor.memberID.size();
				GParkController->Rides[ride].CurrentRiders += Visitor.memberID.size();

				count += Visitor.memberID.size();
			}
			else
			{
				Visitor.group = Constants::kNotValidGroup;
			}

		} while (count < GParkController->Rides[ride].RideThroughput.perMinuteIFastPass && Visitor.group != Constants::kNotValidGroup);
	}

	do
	{
		Visitor = GParkController->Rides[ride].NextItemInQueue();

		// if this group is larger than the maximum throughput per minute then we have to let them all through
		if (Visitor.group != Constants::kNotValidGroup && (static_cast<int>(Visitor.memberID.size()) <= (GParkController->Rides[ride].RideThroughput.perMinuteI + GParkController->Rides[ride].RideThroughput.perMinuteIFastPass - count) || static_cast<int>(Visitor.memberID.size()) > GParkController->Rides[ride].RideThroughput.perMinuteI))
		{
			GParkController->Rides[ride].RemoveFromQueue();

			GVisitorController->Groups[Visitor.group].SetRiding(Visitor, ride, GParkController->Rides[ride].RideOperation.rideLength, false);

			GParkController->Rides[ride].DailyStatistics.totalRiders += Visitor.memberID.size();
			GParkController->Rides[ride].CurrentRiders += Visitor.memberID.size();

			count += Visitor.memberID.size();
		}
		else
		{
			Visitor.group = Constants::kNotValidGroup;
		}

	} while (count < GParkController->Rides[ride].RideThroughput.perMinuteI + GParkController->Rides[ride].RideThroughput.perMinuteIFastPass && Visitor.group != Constants::kNotValidGroup);
}


void Engine::RideShowProcessQueue(int ride)
{
	auto count = 0;

	QWaitTypes::Riders Visitor;

	if (GParkController->Rides[ride].FastPass.mode != 0)
	{
		do
		{
			Visitor = GParkController->Rides[ride].RemoveFromQueueFastPass();

			if (Visitor.group != Constants::kNotValidGroup)
			{
				
				GVisitorController->Groups[Visitor.group].SetRiding(Visitor, ride, GParkController->Rides[ride].RideOperation.rideLength, true);

				GParkController->Rides[ride].DailyStatistics.totalRiders += Visitor.memberID.size();
				GParkController->Rides[ride].CurrentRiders += Visitor.memberID.size();

				count += Visitor.memberID.size();
			}

		} while (count < GParkController->Rides[ride].RideThroughput.showCapacityFastPass && Visitor.group != Constants::kNotValidGroup);
	}

	do
	{
		Visitor = GParkController->Rides[ride].RemoveFromQueue();

		if (Visitor.group != Constants::kNotValidGroup)
		{
			GVisitorController->Groups[Visitor.group].SetRiding(Visitor, ride, GParkController->Rides[ride].RideOperation.rideLength, false);

			GParkController->Rides[ride].DailyStatistics.totalRiders += Visitor.memberID.size();
			GParkController->Rides[ride].CurrentRiders += Visitor.memberID.size();

			count += Visitor.memberID.size();
		}

	} while (count < GParkController->Rides[ride].RideThroughput.showCapacity && Visitor.group != Constants::kNotValidGroup);
}


void Engine::RideEmptyQueue(int ride)
{
	if (GParkController->Rides[ride].QueueSize() != 0)
	{
		QWaitTypes::Riders Visitor;

		do
		{
			Visitor = GParkController->Rides[ride].RemoveFromQueue();

			if (Visitor.group != Constants::kNotValidGroup)
			{
				GVisitorController->Groups[Visitor.group].GroupRemovedFromRideShutdown(GParkController->Rides[ride].RideOperation.rideExitType);
			}

		} while (Visitor.group != Constants::kNotValidGroup);
	}

	if (GParkController->Rides[ride].QueueSizeFastPass() != 0)
	{
		QWaitTypes::Riders Visitor;

		do
		{
			Visitor = GParkController->Rides[ride].RemoveFromQueueFastPass();

			if (Visitor.group != Constants::kNotValidGroup)
			{
				GVisitorController->Groups[Visitor.group].GroupRemovedFromRideShutdown(GParkController->Rides[ride].RideOperation.rideExitType);
			}

		} while (Visitor.group != Constants::kNotValidGroup);
	}
}


// =================================================================================================================
// == Food Drink Processing ========================================================================================
// =================================================================================================================


void Engine::EateryProcessQueue(int eatery)
{
	if (GParkController->Eateries[eatery].Queue.size() != 0)
	{
		auto count = 0;

		int group;

		do
		{
			group = GParkController->Eateries[eatery].RemoveFromQueue();

			if (group != Constants::kNotValidGroup)
			{
				GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Eating, VisitorParkStatus::Eating);

				GVisitorController->Groups[group].Behaviour.waitingTime += GParkController->Eateries[eatery].GetWaitTime(GVisitorController->Groups[group].Configuration.type);

				GParkController->Eateries[eatery].Statistics.Customers += GVisitorController->Groups[group].Visitors.size();

				GParkController->Eateries[eatery].Statistics.Spend += (GVisitorController->Groups[group].AdultCount * GParkController->Eateries[eatery].AveragePriceAdult) +
																	  (GVisitorController->Groups[group].ChildCount * GParkController->Eateries[eatery].AveragePriceChild);

				count++;
			}

		} while (count < GParkController->Eateries[eatery].Throughput && group != Constants::kNotValidGroup);
	}
}


void Engine::EateryEmptyQueue(int eatery)
{
	if (GParkController->Eateries[eatery].Queue.size() != 0)
	{
		int group = Constants::kNotValidGroup;

		do
		{
			group = GParkController->Eateries[eatery].RemoveFromQueue();

			if (group != Constants::kNotValidGroup)
			{
				GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);
			}

		} while (group != Constants::kNotValidGroup);
	}
}


// =================================================================================================================
// == Visitor status processing ====================================================================================
// =================================================================================================================


void Engine::ParkStatusEntrance(int group)
{
	GParkController->AddToEntranceQueue(group);

	GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Queuing, VisitorParkStatus::Queuing);

	VisitorsInPark += GVisitorController->Groups[group].Visitors.size();
}


void Engine::ParkStatusIdle(int group)
{
	if ((CurrentTime.minutes >= GVisitorController->Groups[group].Configuration.departureTime.minutes && CurrentTime.hours == GVisitorController->Groups[group].Configuration.departureTime.hours)
		|| !ParkOpen)
	{
		GVisitorController->Groups[group].UpdateLocation(Constants::kLocationExitedPark, Constants::kLocationExitedPark);

		VisitorsInPark -= GVisitorController->Groups[group].Visitors.size();
	}
	else
	{
		bool FindRide = true;

		if (HandleFoodDrink)
		{
			if (GVisitorController->Groups[group].Behaviour.consumption.Drink > GVisitorController->Groups[group].Behaviour.consumption.Threshold)
			{
				QWaitTypes::GetEatery eatery = GParkController->GetClosestEatery(GVisitorController->Groups[group].Behaviour.location, GVisitorController->Groups[group].GetMaximumWaitingTimeFood());

				if (eatery.eatery != -1)
				{
					GVisitorController->Groups[group].SetNewEatery(eatery.eatery,
						static_cast<int>((double)eatery.distance / (double)Constants::WalkSpeedMetresPerMinute),
						eatery.distance,
						eatery.location);

					FindRide = false;
				}
				else
				{
					GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::EateryQueueTooLong);
				}
			}
		}

		if (FindRide)
		{
			// lets find something to ride
			QWaitTypes::GetRide NewRide = GetRide(group);

			if (NewRide.ride != Constants::kNoSelectedRide)
			{
				GVisitorController->Groups[group].SetNewRide(NewRide.ride, NewRide.fastPassTicket,
					GParkController->GetDistanceBetweenInMinutes(GVisitorController->Groups[group].Behaviour.currentRide, NewRide.ride),
					GParkController->GetDistanceBetweenInMetres(GVisitorController->Groups[group].Behaviour.currentRide, NewRide.ride),
					GParkController->Rides[NewRide.ride].RideOperation.position);
			}
			else
			{
				GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NoRideAvailable);

				GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentIdle);
			}
		}
	}
}


void Engine::ParkStatusRiding(int group)
{
	if (GVisitorController->Groups[group].Riding.timeLeft == 0)
	{
		int ridercount = GVisitorController->Groups[group].GroupRemovedFromRide(GVisitorController->Groups[group].Behaviour.currentRide, GParkController->Rides[GVisitorController->Groups[group].Behaviour.currentRide].RideOperation.rideExitType);

		GParkController->Rides[GVisitorController->Groups[group].Behaviour.currentRide].CurrentRiders -= ridercount;

		// fastpass stuff
		if (FastPassMode != FastPassType::None && GVisitorController-> Groups[group].FastPassTickets.size() == 0)
		{
			GetReplacementFastPassRide(group);

			GVisitorController->Groups[group].SetWaiting(5); // indicates time taken to get/check fastpass tickets
		}
	}
	else
	{
		GVisitorController->Groups[group].Riding.timeLeft--;

		GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentRiding);
	}
}


void Engine::ParkStatusTravellingRide(int group)
{
	if (GVisitorController->Groups[group].Behaviour.travelling.minutesLeft == 0)
	{
		GVisitorController->Groups[group].UpdateLocation(GParkController->Rides[GVisitorController->Groups[group].Behaviour.travelling.toRide].RideOperation.position.x,
			                                             GParkController->Rides[GVisitorController->Groups[group].Behaviour.travelling.toRide].RideOperation.position.y);

		if (ParkOpen)
		{
			int Ride = GVisitorController->Groups[group].Behaviour.travelling.toRide;

			// if they travel there and find the queue has changed then they are willing to wait a bit longer
			if (GParkController->Rides[Ride].WaitTime(GVisitorController->Groups[group].Behaviour.travelling.fastPass) < (double)GVisitorController->Groups[group].Behaviour.maximumRideWaitingTime * 1.10)
			{
				QWaitTypes::Riders riders;

				riders.group = group;

				for (int v = 0; v < GVisitorController->Groups[group].Visitors.size(); v++)
				{
					if (GVisitorController->Groups[group].IsRideSuitable(v, GParkController->Rides[Ride].RideOperation.ChildValid, GParkController->Rides[Ride].RideOperation.AdultValid)) // to do fail here?!
					{
						riders.memberID.push_back(v);

						if (GVisitorController->Groups[group].Behaviour.travelling.fastPass != Constants::kNoFastPass)
						{
							GVisitorController->Groups[group].Visitors[v].SetQueuingFastPassLengthStat(GParkController->Rides[Ride].WaitTime(GVisitorController->Groups[group].Behaviour.travelling.fastPass));
						}
						else
						{
							GVisitorController->Groups[group].Visitors[v].SetQueuingLengthStat(GParkController->Rides[Ride].WaitTime(GVisitorController->Groups[group].Behaviour.travelling.fastPass));
						}
					}
					else
					{
						GVisitorController->Groups[group].Visitors[v].Rides.rideNotSuitableForMe++;
					}
				}

				if (riders.memberID.size() != 0)
				{
					if (GVisitorController->Groups[group].Behaviour.travelling.fastPass != Constants::kNoFastPass)
					{
						GParkController->Rides[Ride].AddToQueueFastPass(riders);

						GVisitorController->Groups[group].SetAtRideQueuingFastPass(Ride);
					}
					else
					{
						GParkController->Rides[Ride].AddToQueue(riders);

						GVisitorController->Groups[group].SetAtRideQueuing(Ride);
					}
				}
				else
				{
					GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);

					GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::NooneCanRideInGroup);
				}
			}
			else
			{			
				// queue is much bigger than expected or tolerated, so let's go looking for something else
				GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);

				GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::WaitTimeTooLong);
			}
		}
		else
		{
			GVisitorController->Groups[group].UpdateLocation(Constants::kLocationExitedPark, Constants::kLocationExitedPark);
		}
	}
	else
	{	
		GVisitorController->Groups[group].Behaviour.travelling.minutesLeft--;

		GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentTravelling);		

		GVisitorController->Groups[group].UpdateTravellingLocation();
	}
}


void Engine::ParkStatusWaiting(int group)
{
	if ((CurrentTime.minutes >= GVisitorController->Groups[group].Configuration.departureTime.minutes && CurrentTime.hours == GVisitorController->Groups[group].Configuration.departureTime.hours)
		|| !ParkOpen)
	{
		GVisitorController->Groups[group].UpdateLocation(Constants::kLocationExitedPark, Constants::kLocationExitedPark);

		VisitorsInPark -= GVisitorController->Groups[group].Visitors.size();
	}
	else
	{
		if (GVisitorController->Groups[group].Behaviour.waitingTime == 0)
		{
			GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);
		}
		else
		{
			GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentWaiting);

			GVisitorController->Groups[group].Behaviour.waitingTime--;
		}
	}
}


void Engine::ParkStatusTravellingFood(int group)
{
	if (GVisitorController->Groups[group].Behaviour.travelling.minutesLeft == 0)
	{
		GVisitorController->Groups[group].UpdateLocation(GParkController->Eateries[GVisitorController->Groups[group].Behaviour.travelling.toEatery].X,
			GParkController->Eateries[GVisitorController->Groups[group].Behaviour.travelling.toEatery].Y);

		if (ParkOpen)
		{
			int Eatery = GVisitorController->Groups[group].Behaviour.travelling.toEatery;

			GParkController->Eateries[Eatery].AddToQueue(group);

			GVisitorController->Groups[group].SetAtEateryQueuing(Eatery);
		}
		else
		{
			GVisitorController->Groups[group].UpdateLocation(Constants::kLocationExitedPark, Constants::kLocationExitedPark);
		}
	}
	else
	{
		GVisitorController->Groups[group].Behaviour.travelling.minutesLeft--;

		GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentTravellingFood);// to do

		GVisitorController->Groups[group].UpdateTravellingLocation();
	}
}


void Engine::ParkStatusEating(int group)
{
	if ((CurrentTime.minutes >= GVisitorController->Groups[group].Configuration.departureTime.minutes && CurrentTime.hours == GVisitorController->Groups[group].Configuration.departureTime.hours)
		|| !ParkOpen)
	{
		GVisitorController->Groups[group].UpdateLocation(Constants::kLocationExitedPark, Constants::kLocationExitedPark);

		VisitorsInPark -= GVisitorController->Groups[group].Visitors.size();
	}
	else
	{
		if (GVisitorController->Groups[group].Behaviour.waitingTime == 0)
		{
			GVisitorController->Groups[group].SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);

			GVisitorController->Groups[group].ResetFoodDrink();
		}
		else
		{
			GVisitorController->Groups[group].SetStatForAllVisitors(GroupVisitorStat::TimeSpentEating);

			GVisitorController->Groups[group].Behaviour.waitingTime--;
		}
	}
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


void Engine::ShowLiveStats()
{
	size_t Queue = 0;
	size_t QueueFP = 0;
	int Riding = 0;
	size_t AtRide = 0;
	std::wstring log = L"";

	for (int t = 0; t < GParkController->Rides.size(); t++)
	{
		Queue += GParkController->Rides[t].QueueSize();
		QueueFP += GParkController->Rides[t].QueueSizeFastPass();
		Riding += GParkController->Rides[t].CurrentRiders;
	}

	AtRide = Queue + QueueFP + Riding;

	int pcriding = 0;

	if (VisitorsInPark != 0)
	{
		pcriding = static_cast<int>(((double)AtRide / (double)VisitorsInPark) * 100.0);
	}

	if (FastPassMode == FastPassType::None)
	{ 
		log = Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes) + L"  " + Utility::PadRight(VisitorsInPark, 6) + L" q: " + Utility::PadRight(Queue, 6) + L" r: " + Utility::PadRight(Riding, 6) + L"   (" + Utility::PadRight(AtRide, 6) + L" / " + Utility::PadRight(VisitorsInPark, 6) + L") " + ReportUtility::BarGraph(pcriding);
	}
	else
	{
		log = Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes) + L"  " + Utility::PadRight(VisitorsInPark, 6) + L" q: " + Utility::PadRight(Queue, 6) + L" qfp: " + Utility::PadRight(QueueFP, 6) + L" r: " + Utility::PadRight(Riding, 6) + L"   (" + Utility::PadRight(AtRide, 6) + L" / " + Utility::PadRight(VisitorsInPark, 6) + L") " + ReportUtility::BarGraph(pcriding);
	}

	OutputStatus(L"    " + log);

	GParkController->Log.push_back(log);
}