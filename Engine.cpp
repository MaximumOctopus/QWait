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
#include <random>
#include "Constants.h"
#include "Engine.h"
#include "RideController.h"
#include "Utility.h"
#include "VisitorController.h"


extern RideController* GRideController;
extern VisitorController* GVisitorController;


Engine::Engine(int fast_pass, int park_open_time, int park_close_time, bool show_output)
{
	ParkOpen = true;

	ShowOutput = show_output;

	FastPassMode = fast_pass;

	GRideController->FastPassMode = FastPassMode;

	CurrentTime.hours = park_open_time;
	CurrentTime.minutes = 0;

	TimeRidesClose = park_close_time;
	TimeParkCloses = park_close_time + 1;

	VisitorsInPark = 0;
}


void Engine::OutputStatus(std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


void Engine::Run(bool report_mxm, bool report_visitor_location)
{
	if (FastPassMode != Constants::FastPassModeNone)
	{
		PreArrivalFastPass();
	}

	OutputStatus("Park open!");

	while (CurrentTime.hours != TimeParkCloses)
	{
		// ========================================================================================================
		// == Visitor Processing ==================================================================================
		// ========================================================================================================

		for (int v = 0; v < GVisitorController->Visitors.size(); v++)
		{
			switch (GVisitorController->Visitors[v].ParkStatus)
			{
				// let's see if the visitor should enter the park
			case Constants::ParkStatusOnWay:

				if (Utility::IsTimeEqual(CurrentTime, GVisitorController->Visitors[v].ArrivalTime))
				{
					GVisitorController->Visitors[v].ParkStatus = Constants::ParkStatusAtEntrance;

					GVisitorController->Visitors[v].UpdateLocation(GRideController->entrance.position.x, 
						                                           GRideController->entrance.position.y);
				}

				break;

			case Constants::ParkStatusAtEntrance:

				ParkStatusEntrance(v);

				break;

			case Constants::ParkStatusIdle:

				ParkStatusIdle(v);

				break;

			case Constants::ParkStatusRiding:

				ParkStatusRiding(v);

				break;

			case Constants::ParkStatusQueuing:
			case Constants::ParkStatusQueuingFastPass:

				GVisitorController->Visitors[v].TimeSpent.queuing++;

				break;

			case Constants::ParkStatusTravelling:

				ParkStatusTravelling(v);

				break;

			case Constants::ParkStatusWaiting:

				ParkStatusWaiting(v);

				break;

			case Constants::ParkStatusExited:

				break;
			}

			if (report_visitor_location)
			{
				GVisitorController->Visitors[v].SaveMinuteStats();
			}
		}

		// ========================================================================================================
		// == Entrance Queue Processing ===========================================================================
		// ========================================================================================================

		if (GRideController->EntranceQueueSize() != 0)
		{
			EntranceProcessQueue();
		}

		// ========================================================================================================
		// == Ride Processing =====================================================================================
		// ========================================================================================================

		for (int r = 0; r < GRideController->Rides.size(); r++)
		{
			if (!GRideController->Rides[r].RideOperation.isShutdown)
			{
				ProcessRide(r);
			}

			if (report_mxm)
			{
				GRideController->Rides[r].UpdateMinuteStats();
			}
		}

		if (report_mxm)
		{
			GVisitorController->UpdateMinuteStats(Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes));
		}

		// ========================================================================================================

		GRideController->Minutes++;

		// ========================================================================================================

		UpdateClock();
	}

	GRideController->Minutes--;

	OutputStatus("Park closed!");
}


void Engine::UpdateClock()
{
	if (CurrentTime.minutes % 15 == 0)
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


QWaitTypes::GetRide Engine::GetRide(int visitor)
{
	int SelectedRide = kNoSelectedRide;
	size_t CheckingRides = GRideController->Rides.size() * 2;

	// == ride selection "magic" ================================================

	while (CheckingRides != 0)
	{
		int num = rand() % kSelectionChoiceCacheCount;

		SelectedRide = GRideController->SelectionChoiceCache[GVisitorController->Visitors[visitor].Type][num];

		if (GRideController->Rides[SelectedRide].WaitTime(kNoSelectedFastPassTicket) < GVisitorController->Visitors[visitor].Rides.maxWaitTime)
		{
			int isbetteravailable = IsRideAvailableCloser(visitor, SelectedRide);

			QWaitTypes::FastPass FastPassDetails = IsFastPassTicketSoon(visitor, SelectedRide);

			if (isbetteravailable != kNoSelectedRide && FastPassDetails.ride == kNoSelectedRide)
			{
				return { isbetteravailable, kNoSelectedFastPassTicket };
			}
			else if (FastPassDetails.ride != kNoSelectedRide)
			{
				return { FastPassDetails.ride, FastPassDetails.ticket.hours };
			}

			return { SelectedRide, kNoSelectedFastPassTicket };
		}
		else
		{
		}

		CheckingRides--;
	}

	// ==========================================================================

	return { kNoDestinationRide, kNoSelectedFastPassTicket };
}


// if there's a ride which has better popularity, lower wait time, and is closer, then select this instead
int Engine::IsRideAvailableCloser(int visitor, int possible_destination)
{
	if (GVisitorController->Visitors[visitor].Rides.current == kNoCurrentRide)
	{
		return kNoSelectedRide;
	}

	int CurrentRide = GVisitorController->Visitors[visitor].Rides.current;

	Ride r = GRideController->Rides[CurrentRide];
	Ride d = GRideController->Rides[possible_destination];

	for (int i = 0; i < 5; i++)
	{
		Ride c = GRideController->Rides[r.ClosestCache[i]];

		if (c.RideOperation.Popularity > d.RideOperation.Popularity &&
			GRideController->GetDistanceBetweenInMinutes(r.ClosestCache[i], CurrentRide) < GRideController->GetDistanceBetweenInMinutes(possible_destination, CurrentRide) &&
			c.WaitTime(kNoSelectedFastPassTicket) < d.WaitTime(kNoSelectedFastPassTicket) &&
			r.ClosestCache[i] != GVisitorController->Visitors[visitor].LastThreeRides[2] &&
			r.ClosestCache[i] != GVisitorController->Visitors[visitor].LastThreeRides[1])
		{
			return r.ClosestCache[i];
		}
	}

	return kNoSelectedRide;
}


QWaitTypes::FastPass Engine::IsFastPassTicketSoon(int visitor, int possible_destination)
{
	if (FastPassMode != Constants::FastPassModeNone)
	{
		if (GVisitorController->Visitors[visitor].FastPassTickets.size() != 0)
		{
			int TimeRequired = static_cast<int>(GRideController->Rides[possible_destination].WaitTime(kNoSelectedFastPassTicket)) +
		                                 		GRideController->GetDistanceBetweenInMinutes(GVisitorController->Visitors[visitor].Rides.current, possible_destination) +
		                                   		GRideController->GetDistanceBetweenInMinutes(possible_destination, GVisitorController->Visitors[visitor].FastPassTickets[0].ride);

			int TimeToFPT = Utility::Duration(CurrentTime, GVisitorController->Visitors[visitor].FastPassTickets[0].ticket);

			// fastpass slot is 1 hour
			if (TimeRequired < TimeToFPT + 59)
			{
				// plenty of time to get to this ride
			}
			else
			{
				return GVisitorController->Visitors[visitor].FastPassTickets[0];
			}
		}
	}

	return { kNoSelectedType, kNoSelectedRide, { 0, 0 } };
}


// =================================================================================================================
// == FastPass Processing ==========================================================================================
// =================================================================================================================


void Engine::PreArrivalFastPass()
{
	size_t count = 0;

	if (FastPassMode != Constants::FastPassModePlusNoStayBenefit)
	{
		// Pass 1, visitors who are "staying at the park"

		OutputStatus("Processing FastPass tickets...");

		for (int v = 0; v < GVisitorController->Visitors.size(); v++)
		{
			if (GVisitorController->Visitors[v].StayingOnSite)
			{
				ProcessFastPassesForVisitor(v);

				count += GVisitorController->Visitors[v].FastPassTickets.size();
			}
		}
	}

	GVisitorController->FastPassStats.ticketsEarly = count;

	// Pass 2, everyone else! =========================================================

	for (int v = 0; v < GVisitorController->Visitors.size(); v++)
	{
		if (!GVisitorController->Visitors[v].StayingOnSite || FastPassMode == Constants::FastPassModePlusNoStayBenefit)
		{
			ProcessFastPassesForVisitor(v);

			count += GVisitorController->Visitors[v].FastPassTickets.size();
		}

		GVisitorController->Visitors[v].SortFastPassTickets();
	}

	GVisitorController->FastPassStats.ticketsEarlyAll = count;
}


void Engine::ProcessFastPassesForVisitor(int visitor)
{
	QWaitTypes::FastPass FP1 = GetFastPassRide(1, GVisitorController->Visitors[visitor].ArrivalTime, GVisitorController->Visitors[visitor].DepartureTime);

	if (FP1.ride != kNoSelectedRide)
	{
		GVisitorController->Visitors[visitor].AddFastPassTicket(FP1.type, FP1.ride, FP1.ticket);
	}
	else
	{
		GVisitorController->Visitors[visitor].Rides.noFastPassRideForMe++;
	}

	for (int x = 0; x < 2; x++)
	{
		QWaitTypes::FastPass FP2 = GetFastPassRide(2, GVisitorController->Visitors[visitor].ArrivalTime, GVisitorController->Visitors[visitor].DepartureTime);

		if (FP2.ride != kNoSelectedRide)
		{
			GVisitorController->Visitors[visitor].AddFastPassTicket(FP2.type, FP2.ride, FP2.ticket);
		}
		else
		{
			GVisitorController->Visitors[visitor].Rides.noFastPassRideForMe++;
		}
	}

	for (int x = 0; x < 3; x++)
	{
		QWaitTypes::FastPass FP3 = GetFastPassRide(3, GVisitorController->Visitors[visitor].ArrivalTime, GVisitorController->Visitors[visitor].DepartureTime);

		if (FP3.ride != kNoSelectedRide)
		{
			GVisitorController->Visitors[visitor].AddFastPassTicket(FP3.type, FP3.ride, FP3.ticket);
		}
		else
		{
			GVisitorController->Visitors[visitor].Rides.noFastPassRideForMe++;
		}
	}
}


// guests allowed 1 type 1, 2 from type 2, 3 from type 3 (if available)
QWaitTypes::FastPass Engine::GetFastPassRide(int type, QWaitTypes::Time arrive, QWaitTypes::Time leave)
{
	if (GRideController->FastPassType[type - 1].size() != 0)
	{
		int fpt = 0;
		int ticket = kNoSelectedFastPassTicket;

		do
		{
			int ride = GRideController->FastPassType[type - 1][fpt];
			
			ticket = GRideController->Rides[ride].ViewFastPassTicket(arrive, leave);

			if (ticket != kNoSelectedFastPassTicket)
			{
				if (Utility::IsTicketValidForTime(arrive, leave, { ticket, 0 }))
				{
					return { type, ride, { GRideController->Rides[ride].GetFastPassTicket(arrive, leave), 0 } };
				}

				ticket = kNoSelectedFastPassTicket;
			}

			fpt++;

		} while (fpt < GRideController->FastPassType[type - 1].size() && ticket == kNoSelectedFastPassTicket);
	}

	return { kNoSelectedType, kNoSelectedRide, { 0, 0 } };
}


void Engine::GetReplacementFastPassRide(int visitor)
{
	if (GVisitorController->Visitors[visitor].FastPassTickets.size() == 0)
	{
		if (GRideController->entrance.fastPassType == 1)
		{
			QWaitTypes::FastPass FP1 = GetFastPassRide(1, CurrentTime, GVisitorController->Visitors[visitor].DepartureTime);

			if (FP1.ride != kNoSelectedRide)
			{
				GVisitorController->Visitors[visitor].AddFastPassTicket(FP1.type, FP1.ride, FP1.ticket);

				return;
			}

			QWaitTypes::FastPass FP2 = GetFastPassRide(2, CurrentTime, GVisitorController->Visitors[visitor].DepartureTime);

			if (FP2.ride != kNoSelectedRide)
			{
				GVisitorController->Visitors[visitor].AddFastPassTicket(FP2.type, FP2.ride, FP2.ticket);

				return;
			}
		}
		else if (GRideController->entrance.fastPassType == 3)
		{
			QWaitTypes::FastPass FP3 = GetFastPassRide(3, CurrentTime, GVisitorController->Visitors[visitor].DepartureTime);

			if (FP3.ride != kNoSelectedRide)
			{
				GVisitorController->Visitors[visitor].AddFastPassTicket(FP3.type, FP3.ride, FP3.ticket);

				return;
			}
		}

		GVisitorController->Visitors[visitor].Rides.noFastPassRideForMe++;
	}
}


// =================================================================================================================
// == Ride status processing =======================================================================================
// =================================================================================================================


void Engine::EntranceProcessQueue()
{
	int count = 0;
	int VisitorID = kNotValidVisitor;

	do
	{
		VisitorID = GRideController->RemoveFromEntranceQueue();

		if (VisitorID != kNotValidVisitor)
		{
			GVisitorController->Visitors[VisitorID].ParkStatus = Constants::ParkStatusIdle;
		}

		count++;

	} while (count < GRideController->entrance.throughPutMinute && VisitorID != kNotValidVisitor);
}


void Engine::ProcessRide(int ride)
{
	if (GRideController->Rides[ride].RideOperation.isOpen)
	{
		if (Utility::IsTimeEqual(CurrentTime, GRideController->Rides[ride].RideOperation.close))
		{
			RideEmptyQueue(ride);

			GRideController->Rides[ride].Close();

			return;
		}
	}
	else
	{
		if (Utility::IsTimeEqual(CurrentTime, GRideController->Rides[ride].RideOperation.open))
		{
			GRideController->Rides[ride].RideOperation.isOpen = true;
		}
	}

	switch (GRideController->Rides[ride].RideOperation.rideType)
	{
	case kRideTypeContinuous:
	{
		if (GRideController->Rides[ride].QueueSize() != 0 || GRideController->Rides[ride].QueueSizeFastPass() != 0)
		{
			RideContinuousProcessQueue(ride);
		}

		break;
	}
	case kRideTypeShow:
	{
		if ((GRideController->Rides[ride].QueueSize() != 0 || GRideController->Rides[ride].QueueSizeFastPass() != 0) && CurrentTime.minutes % GRideController->Rides[ride].RideOperation.ShowStartTime == 0)
		{
			RideShowProcessQueue(ride);
		}

		break;
	}
	}

	GRideController->Rides[ride].UpdateDailyStatistics();
}


void Engine::RideContinuousProcessQueue(int ride)
{
	int count = 0;
	int VisitorID = kNotValidVisitor;

	if (GRideController->Rides[ride].FastPass.mode != 0)
	{
		do
		{
			VisitorID = GRideController->Rides[ride].RemoveFromQueueFastPass();
			   
			if (VisitorID != kNotValidVisitor)
			{
				GVisitorController->Visitors[VisitorID].SetRiding(ride, GRideController->Rides[ride].RideOperation.rideLength, true);

				GRideController->Rides[ride].DailyStatistics.totalRiders++;
				GRideController->Rides[ride].CurrentRiders++;
			}

			count++;

		} while (count < GRideController->Rides[ride].RideThroughput.perMinuteIFastPass && VisitorID != kNotValidVisitor);
	}

	do
	{
		VisitorID = GRideController->Rides[ride].RemoveFromQueue();

		if (VisitorID != kNotValidVisitor)
		{
			GVisitorController->Visitors[VisitorID].SetRiding(ride, GRideController->Rides[ride].RideOperation.rideLength, false);

			GRideController->Rides[ride].DailyStatistics.totalRiders++;
			GRideController->Rides[ride].CurrentRiders++;
		}

		count++;

	} while (count < GRideController->Rides[ride].RideThroughput.perMinuteI + GRideController->Rides[ride].RideThroughput.perMinuteIFastPass && VisitorID != kNotValidVisitor);
}


void Engine::RideShowProcessQueue(int ride)
{
	int count = 0;
	int VisitorID = kNotValidVisitor;

	if (GRideController->Rides[ride].FastPass.mode != 0)
	{
		do
		{
			VisitorID = GRideController->Rides[ride].RemoveFromQueueFastPass();

			if (VisitorID != kNotValidVisitor)
			{
				GVisitorController->Visitors[VisitorID].SetRiding(ride, GRideController->Rides[ride].RideOperation.rideLength, true);

				GRideController->Rides[ride].DailyStatistics.totalRiders++;
				GRideController->Rides[ride].CurrentRiders++;
			}

			count++;

		} while (count < GRideController->Rides[ride].RideThroughput.showCapacityFastPass && VisitorID != kNotValidVisitor);
	}

	do
	{
		VisitorID = GRideController->Rides[ride].RemoveFromQueue();

		if (VisitorID != kNotValidVisitor)
		{
			GVisitorController->Visitors[VisitorID].SetRiding(ride, GRideController->Rides[ride].RideOperation.rideLength, false);

			GRideController->Rides[ride].DailyStatistics.totalRiders++;
			GRideController->Rides[ride].CurrentRiders++;
		}

		count++;

	} while (count < GRideController->Rides[ride].RideThroughput.showCapacity && VisitorID != kNotValidVisitor);
}


void Engine::RideEmptyQueue(int ride)
{
	if (GRideController->Rides[ride].QueueSize() != 0)
	{
		int VisitorID = kNotValidVisitor;

		do
		{
			VisitorID = GRideController->Rides[ride].RemoveFromQueue();

			if (VisitorID != kNotValidVisitor)
			{
				GVisitorController->Visitors[VisitorID].ParkStatus = Constants::ParkStatusIdle;
				
				GVisitorController->Visitors[VisitorID].Rides.rideShutdown++;
			}

		} while (VisitorID != kNotValidVisitor);
	}

	if (GRideController->Rides[ride].QueueSizeFastPass() != 0)
	{
		int VisitorID = kNotValidVisitor;

		do
		{
			VisitorID = GRideController->Rides[ride].RemoveFromQueueFastPass();

			if (VisitorID != kNotValidVisitor)
			{
				GVisitorController->Visitors[VisitorID].ParkStatus = Constants::ParkStatusIdle;

				GVisitorController->Visitors[VisitorID].Rides.rideShutdown++;
			}

		} while (VisitorID != kNotValidVisitor);
	}
}


// =================================================================================================================
// == Visitor status processing ====================================================================================
// =================================================================================================================


void Engine::ParkStatusEntrance(int visitor)
{
	GRideController->AddToEntranceQueue(visitor);

	GVisitorController->Visitors[visitor].ParkStatus = Constants::ParkStatusQueuing;

	VisitorsInPark++;
}


void Engine::ParkStatusIdle(int visitor)
{
	if ((CurrentTime.minutes >= GVisitorController->Visitors[visitor].DepartureTime.minutes && CurrentTime.hours == GVisitorController->Visitors[visitor].DepartureTime.hours)
		|| !ParkOpen)
	{
		GVisitorController->Visitors[visitor].UpdateLocation(kLocationExitedPark, kLocationExitedPark);

		VisitorsInPark--;
	}
	else
	{
		// lets find something to ride
		QWaitTypes::GetRide NewRide = GetRide(visitor);

		if (NewRide.ride != kNoSelectedRide)
		{
			GVisitorController->Visitors[visitor].SetNewRide(NewRide.ride, NewRide.fastPassTicket, 
				                                             GRideController->GetDistanceBetweenInMinutes(GVisitorController->Visitors[visitor].Rides.current, NewRide.ride),
				                                             GRideController->GetDistanceBetweenInMetres(GVisitorController->Visitors[visitor].Rides.current, NewRide.ride),
				                                             GRideController->Rides[NewRide.ride].RideOperation.position);
		}
		else
		{
			GVisitorController->Visitors[visitor].Rides.noRideAvailable++;
			GVisitorController->Visitors[visitor].TimeSpent.idle++;
		}
	}
}


void Engine::ParkStatusRiding(int visitor)
{
	if (GVisitorController->Visitors[visitor].Rides.timeLeft == 0)
	{
		GVisitorController->Visitors[visitor].ParkStatus = Constants::ParkStatusIdle;

		GRideController->Rides[GVisitorController->Visitors[visitor].Rides.current].CurrentRiders--;

		if (FastPassMode != Constants::FastPassModeNone && GVisitorController->Visitors[visitor].FastPassTickets.size() == 0)
		{
			GetReplacementFastPassRide(visitor);

			GVisitorController->Visitors[visitor].SetWaiting(5); // indicates time taken to get/check fastpass tickets
		}
	}
	else
	{
		GVisitorController->Visitors[visitor].Rides.timeLeft--;

		GVisitorController->Visitors[visitor].TimeSpent.riding++;
	}
}


void Engine::ParkStatusTravelling(int visitor)
{
	if (GVisitorController->Visitors[visitor].Travelling.minutesLeft == 0)
	{
		GVisitorController->Visitors[visitor].UpdateLocation(GRideController->Rides[GVisitorController->Visitors[visitor].Travelling.toRide].RideOperation.position.x,
			                                                 GRideController->Rides[GVisitorController->Visitors[visitor].Travelling.toRide].RideOperation.position.y);

		if (ParkOpen)
		{
			// if they travel there and find the queue has changed then they are willing to wait a bit longer
			if (GRideController->Rides[GVisitorController->Visitors[visitor].Travelling.toRide].WaitTime(GVisitorController->Visitors[visitor].Travelling.fastPass) < (float)GVisitorController->Visitors[visitor].Rides.maxWaitTime * 1.10f)
			{
				GVisitorController->Visitors[visitor].Rides.current = GVisitorController->Visitors[visitor].Travelling.toRide;
				GVisitorController->Visitors[visitor].Travelling.toRide = kNoDestinationRide;

				if (GVisitorController->Visitors[visitor].Travelling.fastPass != kNoFastPass)
				{
					GRideController->Rides[GVisitorController->Visitors[visitor].Rides.current].AddToQueueFastPass(visitor);

					GVisitorController->Visitors[visitor].SetQueuingFastPass(GRideController->Rides[GVisitorController->Visitors[visitor].Rides.current].WaitTime(GVisitorController->Visitors[visitor].Travelling.fastPass));
				}
				else
				{
					GRideController->Rides[GVisitorController->Visitors[visitor].Rides.current].AddToQueue(visitor);

					GVisitorController->Visitors[visitor].SetQueuing(GRideController->Rides[GVisitorController->Visitors[visitor].Rides.current].WaitTime(GVisitorController->Visitors[visitor].Travelling.fastPass));
				}

				GVisitorController->Visitors[visitor].UpdateLastRidesList();
			}
			else
			{
				// queue is much bigger than expected or tolerated, so let's go looking for something else
				GVisitorController->Visitors[visitor].ParkStatus = Constants::ParkStatusIdle; // disappointed too

				GVisitorController->Visitors[visitor].Rides.waitTimeTooLong++;
			}
		}
		else
		{
			GVisitorController->Visitors[visitor].UpdateLocation(kLocationExitedPark, kLocationExitedPark);
		}
	}
	else
	{
		GVisitorController->Visitors[visitor].Travelling.minutesLeft--;

		GVisitorController->Visitors[visitor].UpdateTravellingLocation();
	}
}


void Engine::ParkStatusWaiting(int visitor)
{
	if (GVisitorController->Visitors[visitor].WaitingTime == 0)
	{
		GVisitorController->Visitors[visitor].ParkStatus = Constants::ParkStatusIdle;
	}
	else
	{
		GVisitorController->Visitors[visitor].WaitingTime--;
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
	std::string log = "";

	for (int t = 0; t < GRideController->Rides.size(); t++)
	{
		Queue += GRideController->Rides[t].QueueSize();
		QueueFP += GRideController->Rides[t].QueueSizeFastPass();
		Riding += GRideController->Rides[t].CurrentRiders;   
	}

	AtRide = Queue + QueueFP + Riding;

	if (FastPassMode == Constants::FastPassModeNone)
	{ 
		log = Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes) + "  " + Utility::PadRight(VisitorsInPark, 6) + " q: " + Utility::PadRight(Queue, 6) + " r: " + Utility::PadRight(Riding, 6) + "   (" + Utility::PadRight(AtRide, 6) + " / " + Utility::PadRight(VisitorsInPark, 6) + ")";
	}
	else
	{
		log = Utility::FormatTime(CurrentTime.hours, CurrentTime.minutes) + "  " + Utility::PadRight(VisitorsInPark, 6) + " q: " + Utility::PadRight(Queue, 6) + " qfp: " + Utility::PadRight(QueueFP, 6) + " r: " + Utility::PadRight(Riding, 6) + "   (" + Utility::PadRight(AtRide, 6) + " / " + Utility::PadRight(VisitorsInPark, 6) + ")";
	}

	OutputStatus("    " + log);

	GRideController->Log.push_back(log);
}