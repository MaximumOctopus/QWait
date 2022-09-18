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


#include <algorithm>
#include <iostream>
#include <sstream>
#include <time.h>

#include "Constants.h"
#include "Group.h"


// results in a list where Rides[0] is the high popularity
bool sortByTime(const QWaitTypes::FastPass& r1, const QWaitTypes::FastPass& r2)
{
	return r1.ticket.hours < r2.ticket.hours;
}


Group::Group(GroupType group_type, int template_id)
{
	Configuration.templateID = template_id;

	Configuration.type = group_type;

	SetGroupSettings();

	double random = (double)rand() / RAND_MAX;

	switch (Configuration.type)
	{
	case GroupType::Family:
		BuildFamily(random);
		break;
		
	case GroupType::AdultCouple:
		BuildAdultCouple(random);
		break;

	case GroupType::AdultGroup:
		BuildAdultGroup(random);
		break;

	case GroupType::Single:
		BuildSingle(random);
		break;
	}

	Behaviour.maximumRideWaitingTime = GetMaximumWaitingTime();

	Demographics();
}


Group::Group(int type, int template_id, int stay_duration, bool staying_on_site, int arrival_hours, int arrival_minutes, int depart_hours, int depart_minutes)
{
	switch (type)
	{
	case 0:
		Configuration.type = GroupType::Family;
		break;
	case 1:
		Configuration.type = GroupType::AdultCouple;
		break;
	case 2:
		Configuration.type = GroupType::AdultGroup;
		break;
	case 3:
		Configuration.type = GroupType::Single;
		break;
	}

	Configuration.templateID = template_id;

	Configuration.stayDuration = stay_duration;
	Configuration.stayingOnSite = staying_on_site;

	Configuration.arrivalTime = { arrival_hours, arrival_minutes };
	Configuration.departureTime = { depart_hours, depart_minutes };

	Behaviour.maximumRideWaitingTime = GetMaximumWaitingTime();

	Demographics();
}


void Group::Demographics()
{
	for (int t = 0; t < Visitors.size(); t++)
	{
		if (Visitors[t].Configuration.Age == AgeGroup::Adult)
		{
			AdultCount++;
		}
		else if (Visitors[t].Configuration.Age == AgeGroup::Child)
		{
			ChildCount++;
		}
	}
}


void Group::BuildFamily(double random)
{
	int ChildrenCount = 0;

	if (random < kFamilyChildrenCount[0])
	{
		ChildrenCount = 1;
	}
	else if (random < kFamilyChildrenCount[1])
	{
		ChildrenCount = 2;
	}
	else if (random < kFamilyChildrenCount[2])
	{
		ChildrenCount = 3;
	}
	else if (random < kFamilyChildrenCount[3])
	{
		ChildrenCount = 4;
	}
	else
	{
		ChildrenCount = 5;
	}	

	Visitors.push_back(Visitor(GetVisitorGenericAdult()));	// add our two adults
	Visitors.push_back(Visitor(GetVisitorGenericAdult()));

	for (int c = 0; c < ChildrenCount; c++)
	{
		Visitors.push_back(Visitor(GetVisitorGenericChild()));
	}

	Behaviour.consumption.Threshold = kFamilyConsumptionThreshold;
}


void Group::BuildAdultCouple(double random)
{
	double r = ((double)rand() / double(RAND_MAX)) * 0.75;

	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType(r));

	vtc.age = AgeGroup::Adult;

	Visitors.push_back(Visitor(vtc));
	Visitors.push_back(Visitor(vtc));

	Behaviour.consumption.Threshold = kAdultCoupleConsumptionThreshold;
}


void Group::BuildAdultGroup(double random)
{
	int GroupCount = 0;

	if (random < kAdultGroupCount[0])
	{ 
		GroupCount = 2;
	}
	else if (random < kAdultGroupCount[1])
	{
		GroupCount = 3;
	}
	else if (random < kAdultGroupCount[2])
	{
		GroupCount = 4;
	}
	else if (random < kAdultGroupCount[3])
	{
		GroupCount = 5;
	}
	else
	{
		GroupCount = 6;
	}

	for (int g = 0; g < GroupCount; g++)
	{
		Visitors.push_back(Visitor(GetBaseVisitorConfiguration(GetType((double)rand() / double(RAND_MAX)))));
	}

	Behaviour.consumption.Threshold = kAdultGroupConsumptionThreshold;
}


void Group::BuildSingle(double random)
{
	Visitors.push_back(Visitor(GetBaseVisitorConfiguration(GetType((double)rand() / double(RAND_MAX)))));

	Behaviour.consumption.Threshold = kSingleConsumptionThreshold;
}


void Group::SetGroupSettings()
{
	switch (Configuration.type)
	{
	case GroupType::Family:
		Configuration.stayDuration = 11;

		Configuration.arrivalTime.hours = 10 + rand() % 2;
		Configuration.departureTime.hours = Configuration.arrivalTime.hours + Configuration.stayDuration;
		break;

	case GroupType::AdultCouple:
		Configuration.stayDuration = 11;

		Configuration.arrivalTime.hours = 10 + rand() % 3;
		Configuration.departureTime.hours = Configuration.arrivalTime.hours + Configuration.stayDuration;
		break;

	case GroupType::AdultGroup:
		Configuration.stayDuration = 11;

		Configuration.arrivalTime.hours = 10 + rand() % 3;
		Configuration.departureTime.hours = Configuration.arrivalTime.hours + Configuration.stayDuration;
		break;

	case GroupType::Single:
		Configuration.stayDuration = 11;

		Configuration.arrivalTime.hours = 10 + rand() % 5;
		Configuration.departureTime.hours = Configuration.arrivalTime.hours + Configuration.stayDuration;
		break;
	}

	Configuration.arrivalTime.minutes = rand() % 60;
	Configuration.departureTime.minutes = rand() % 60;

	// a certain percentage of groups are staying on-site and get in early
	int OnSite = rand() % 100;

	if (OnSite < Constants::PercentageStayingOnSiteStartingEarly)
	{
		Configuration.stayingOnSite = true;

		Configuration.arrivalTime.hours = 9;
		Configuration.departureTime.hours = Configuration.arrivalTime.hours + Configuration.stayDuration + rand() % 2;

		Configuration.arrivalTime.minutes = rand() % 21; // if you're arriving for 9am you probably aren't going to be too late getting there ;)
	}
	else
	{
		Configuration.stayingOnSite = false;
	}

	Configuration.departureTime.minutes = Configuration.arrivalTime.minutes;
}


VisitorType Group::GetType(double random)
{
	if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypeEnthusiast])
	{
		return VisitorType::Enthusiast;
	}
	else if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypeFan])
	{
		return VisitorType::Fan;
	}
	else if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypeTourist])
	{
		return VisitorType::Tourist;
	}
	else if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypeActivity])
	{
		return VisitorType::Activity;
	}
	else if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypePassholder])
	{
		return VisitorType::Passholder;
	}
	else if (random < kVisitorTypeCoeff[Configuration.templateID][Constants::VisitorTypeEPassHolder])
	{
		return VisitorType::EPassHolder;
	}

	return VisitorType::TikToker;
}


NewVisitorConfiguration Group::GetVisitorGenericAdult()
{
	double r = ((double)rand() / double(RAND_MAX));

	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType(r));

	vtc.age = AgeGroup::Adult;

	return vtc;
}


NewVisitorConfiguration Group::GetVisitorGenericChild()
{
	double r = ((double)rand() / double(RAND_MAX));

	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType(r));

	vtc.age = AgeGroup::Child;

	return vtc;
}


NewVisitorConfiguration Group::GetBaseVisitorConfiguration(VisitorType type)
{
	NewVisitorConfiguration vtc;

	vtc.type = type;

	switch (type)
	{
	case VisitorType::Enthusiast:
		vtc.maxWaitTime = 240.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEnthusiast];

		break;

	case VisitorType::Fan:
		vtc.maxWaitTime = 120.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeFan];

		break;

	case VisitorType::Tourist:
		vtc.maxWaitTime = 90.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeTourist];

		break;

	case VisitorType::Activity:
		vtc.maxWaitTime = 70.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeActivity];

		break;

	case VisitorType::Passholder:
		vtc.maxWaitTime = 60.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypePassholder];

		break;

	case VisitorType::EPassHolder:
		vtc.maxWaitTime = 60.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEPassHolder];

		break;

	case VisitorType::TikToker:
		vtc.maxWaitTime = 45.0;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeTikToker];

		break;
	}

	return vtc;
}


int Group::GetGroupLeaderType()
{
	switch (Configuration.type)
	{
		case GroupType::Family:
			return 0;
			break;
		case GroupType::AdultCouple:
			return 0;
			break;
		case GroupType::AdultGroup:
			return 0;
			break;
		case GroupType::Single:
			return 0;
			break;
	}

	return 0;
}


int Group::GetMaximumWaitingTime()
{
	double mwt = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		mwt += Visitors[v].Rides.maxWaitTime;
	}

	return static_cast<int>((mwt / (double)Visitors.size()) * 1.05);
}


int Group::GetMaximumWaitingTimeFood()
{
	double mwt = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		mwt += Visitors[v].Rides.maxWaitTime;
	}

	return static_cast<int>((mwt / (double)Visitors.size()) * 0.33);
}


void Group::SetStatusForAllVisitors(GroupParkStatus group_park_status, VisitorParkStatus visitor_park_status)
{
	for (int v = 0; v < Visitors.size(); v++)
	{
		Visitors[v].ParkStatus = visitor_park_status;
	}

	Behaviour.parkStatus = group_park_status;
}


// =================================================================================================================
// == Queuing ======================================================================================================
// =================================================================================================================


void Group::SetQueuing(int visitor, double queue_length)
{
	Visitors[visitor].SetQueuingLengthStat(queue_length);
}


void Group::SetQueuingFastPass(int visitor, double queue_length)
{
	Visitors[visitor].SetQueuingFastPassLengthStat(queue_length);
}


// =================================================================================================================
// == Rides ========================================================================================================
// =================================================================================================================


int Group::GetPostRidingWaitTime(RideExitType ride_exit_type, bool first_time)
{
	int WaitTime = 1;

	if (Configuration.type == GroupType::Family)
	{
		WaitTime++;
	}
	else if (Configuration.type == GroupType::AdultGroup)
	{
		WaitTime--;
	}	

	switch (ride_exit_type)
	{
	case RideExitType::None:
		WaitTime += 1;
		break;
	case RideExitType::Short:
		WaitTime += 2;
		break;
	case RideExitType::Long:
		WaitTime += 3;
		break;
	case RideExitType::Shop:
		if (first_time)
		{
			WaitTime += 5;
		}
		else
		{
			WaitTime += 2;
		}
		break;
	case RideExitType::LargeShop:
		if (first_time)
		{
			WaitTime += 6;
		}
		else
		{
			WaitTime += 3;
		}
		break;
	}

	return WaitTime;
}


int Group::GroupRemovedFromRide(int ride, RideExitType ride_exit_type)
{
	int WaitTime = GetPostRidingWaitTime(ride_exit_type, IsFirstTimeRiding(ride));
	int Riding = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		if (Visitors[v].ParkStatus == VisitorParkStatus::Riding)
		{
			Riding++;
		}

		if (WaitTime == 0)
		{
			Visitors[v].ParkStatus = VisitorParkStatus::Idle;
		}
		else
		{
			Visitors[v].ParkStatus = VisitorParkStatus::Waiting;
		}
	}
		
	if (WaitTime == 0)
	{
		SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);
	}
	else
	{
		SetStatusForAllVisitors(GroupParkStatus::Waiting, VisitorParkStatus::Waiting);
	}

	Behaviour.waitingTime += WaitTime;

	return Riding;
}


int Group::GroupRemovedFromRideShutdown(RideExitType ride_exit_type)
{
	int WaitTime = kBasePostRideWaitTime;
	int Riding = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		if (Visitors[v].ParkStatus == VisitorParkStatus::Riding)
		{
			Riding++;
		}

		if (WaitTime == 0)
		{
			Visitors[v].ParkStatus = VisitorParkStatus::Idle;
		}
		else
		{
			Visitors[v].ParkStatus = VisitorParkStatus::Waiting;
		}
	}

	if (WaitTime == 0)
	{
		SetStatusForAllVisitors(GroupParkStatus::Idle, VisitorParkStatus::Idle);
	}
	else
	{
		SetStatusForAllVisitors(GroupParkStatus::Waiting, VisitorParkStatus::Waiting);
	}

	return Riding;
}


void Group::SetRiding(QWaitTypes::Riders riders, int ride_ID, int ride_duration, bool from_fast_pass)
{
	for (int v = 0; v < riders.memberID.size(); v++)
	{
		Visitors[riders.memberID[v]].Rides.currentRide = ride_ID;

		Visitors[riders.memberID[v]].Rides.count++;

		Visitors[riders.memberID[v]].RideList.push_back(ride_ID);

		if (from_fast_pass)
		{
			Visitors[riders.memberID[v]].Rides.fastPassRides++;
		}

		Visitors[riders.memberID[v]].ParkStatus = VisitorParkStatus::Riding;
	}

	if (from_fast_pass)
	{
		FastPassTickets.erase(FastPassTickets.begin());
	}

	RideList.push_back(ride_ID);

	Riding.timeLeft = ride_duration;

	Behaviour.parkStatus = GroupParkStatus::Riding;
}


void Group::SetNewRide(int ride, int fastpass_ticket, int minutes_left, int distance, QWaitTypes::Coords location)
{
	if (ride == Constants::kNoCurrentRide)
	{
		std::cerr << "error -1 ride\n";
	}

	Behaviour.travelling.toRide = ride; 
	Behaviour.travelling.fastPass = fastpass_ticket;

	SetTravellingMinutesLeft(minutes_left, true);

	for (int v = 0; v < Visitors.size(); v++)
	{
		Visitors[v].Rides.distanceTravelled += distance;
	}

	Behaviour.travelling.to.x = location.x;
	Behaviour.travelling.to.y = location.y;
}


void Group::SetTravellingMinutesLeft(int minutes, bool ride)
{
	switch (Configuration.type)
	{
	case GroupType::Family:
		Behaviour.travelling.minutesLeft = minutes + 1;
		break;

	case GroupType::AdultCouple:
		Behaviour.travelling.minutesLeft = minutes;
		break;

	case GroupType::AdultGroup:
		Behaviour.travelling.minutesLeft = minutes;
		break;

	case GroupType::Single:
		Behaviour.travelling.minutesLeft = minutes - 1;
		break;
	}

	Behaviour.travelling.from.x = Behaviour.location.x;
	Behaviour.travelling.from.y = Behaviour.location.y;

	if (Behaviour.travelling.minutesLeft <= 0)
	{
		Behaviour.travelling.minutesLeft = 1;
	}

	Behaviour.travelling.minutesStart = Behaviour.travelling.minutesLeft;

	Statistics.timeSpent.travelling += Behaviour.travelling.minutesLeft;

	if (ride)
	{
		SetStatusForAllVisitors(GroupParkStatus::TravellingRide, VisitorParkStatus::TravellingRide);
	}
	else
	{
		SetStatusForAllVisitors(GroupParkStatus::TravellingFood, VisitorParkStatus::TravellingFood);
	}
}


// can the ride be ridden by the selected visitor
bool Group::IsRideSuitable(int visitor, bool ride_child_valid, bool ride_adult_valid)
{
	switch (Visitors[visitor].Configuration.Age)
	{
	case AgeGroup::Baby:
		break;
	case AgeGroup::Child:
		if (ride_child_valid)
		{
			return true;
		}
		break;
	case AgeGroup::Adult:
		if (ride_adult_valid)
		{
			return true;
		}
		break;
	}

	return false;
}


bool Group::IsFirstTimeRiding(int ride)
{
	for (int r = 0; r < RideList.size(); r++)
	{
		if (RideList[r] == ride)
		{
			return false;
		}
	}

	return true;
}


void Group::SetAtRideQueuing(int ride)
{
	Behaviour.currentRide = ride;

	SetStatusForAllVisitors(GroupParkStatus::Queuing, VisitorParkStatus::Queuing); // TO DO for fastpass code

	Behaviour.travelling.toRide = Constants::kNoDestinationRide;

	UpdateLastRidesList();
}


void Group::SetAtRideQueuingFastPass(int ride)
{
	Behaviour.currentRide = ride;

	SetStatusForAllVisitors(GroupParkStatus::QueuingFastPass, VisitorParkStatus::QueuingFastPass); // TO DO for fastpass code

	Behaviour.travelling.toRide = Constants::kNoDestinationRide;

	UpdateLastRidesList();
}


void Group::UpdateLastRidesList()
{
	Statistics.lastThreeRides[0] = Statistics.lastThreeRides[1];
	Statistics.lastThreeRides[1] = Statistics.lastThreeRides[2];
	Statistics.lastThreeRides[2] = Behaviour.currentRide;
}


// =================================================================================================================
// =================================================================================================================
// =================================================================================================================


void Group::SetNewEatery(int eatery, int minutes_left, int distance, QWaitTypes::Coords location)
{
	Behaviour.travelling.toEatery = eatery;
	Behaviour.travelling.toRide = Constants::kNoCurrentRide;

	SetTravellingMinutesLeft(minutes_left, false);

	for (int v = 0; v < Visitors.size(); v++)
	{
		Visitors[v].Rides.distanceTravelled += distance; // to do? maybe add eatery category!/

		Visitors[v].Rides.eateriesVisited++;             // as above
	}

	Behaviour.travelling.to.x = location.x;
	Behaviour.travelling.to.y = location.y;
}


// =================================================================================================================
// =================================================================================================================
// =================================================================================================================


void Group::AddFastPassTicket(int type, int ride, QWaitTypes::Time entry_time)
{
	QWaitTypes::FastPass fp = { type, ride, entry_time };

	FastPassTickets.push_back(fp);
}


// order fastpass tickets so earliest are first
void Group::SortFastPassTickets()
{
	std::sort(FastPassTickets.begin(), FastPassTickets.end(), sortByTime);
}


// =================================================================================================================
// == Food and Drink ===============================================================================================
// =================================================================================================================


void Group::SetAtEateryQueuing(int eatery)
{
	Behaviour.currentEatery = eatery;

	SetStatusForAllVisitors(GroupParkStatus::QueuingFood, VisitorParkStatus::QueuingFood);

	Behaviour.travelling.toEatery = Constants::kNoDestinationEatery;

	EateryList.push_back(eatery);

	UpdateLastRidesList();
}


void Group::UpdateConsumption(int temp)
{
	double deltat = (0.3333 * (double)Behaviour.consumption.TimeSinceDrink * sqrt(temp));
	double modifier = 1;

	switch (Behaviour.parkStatus)
	{
	case GroupParkStatus::OnWay:
	case GroupParkStatus::AtEntrance:
	case GroupParkStatus::Exited:
	case GroupParkStatus::QueuingFood:
	case GroupParkStatus::Eating:
		return;
	
	case GroupParkStatus::Idle:
	case GroupParkStatus::Waiting:
		modifier = 0.3;
		break;
		
	case GroupParkStatus::Riding:
		modifier = 0.9;
		break;
		
	case GroupParkStatus::Queuing:
	case GroupParkStatus::QueuingFastPass:
		modifier = 0.6;
		break;

	case GroupParkStatus::TravellingRide:
	case GroupParkStatus::TravellingFood:
		modifier = 1;
		break;
	}

	Behaviour.consumption.Drink += modifier * (deltat * deltat);
}


void Group::ResetFoodDrink()
{
	Behaviour.consumption.Drink = 0;
	Behaviour.consumption.Food = 0;
	Behaviour.consumption.TimeSinceDrink = 0;
}


// =================================================================================================================
// =================================================================================================================
// =================================================================================================================


void Group::SetWaiting(int time_minutes)
{
	Behaviour.waitingTime += time_minutes;

	SetStatusForAllVisitors(GroupParkStatus::Waiting, VisitorParkStatus::Waiting);
}


// =================================================================================================================
// == File I/O =====================================================================================================
// =================================================================================================================


bool Group::CreateVisitorFromFileData(const std::wstring input)
{
	std::wstringstream visitorinput(input);
	std::vector<std::wstring> parameters;
	std::wstring parameter;

	while (std::getline(visitorinput, parameter, L','))
	{
		parameters.push_back(parameter);
	}

	if (parameters.size() == 5)
	{
		NewVisitorConfiguration newvisitorconfig;

		try
		{
			newvisitorconfig.name = stoi(parameters[0]);
			newvisitorconfig.type = QWaitTypes::IntToVisitorType(stoi(parameters[1]));

			switch (stoi(parameters[2]))
			{
			case 0:
				newvisitorconfig.age = AgeGroup::Baby;
				break;
			case 1:
				newvisitorconfig.age = AgeGroup::Child;
				break;
			case 2:
				newvisitorconfig.age = AgeGroup::Adult;
				break;
			}

			newvisitorconfig.maxWaitTime = stof(parameters[3]);
			newvisitorconfig.preference = stof(parameters[4]);

			Visitor visitor(newvisitorconfig);

			Visitors.push_back(visitor);

			return true;
		}
		catch (...)
		{
			std::wcerr << L"There appears to be some dodgy data in this input row:" << "\n";
			std::wcerr << L"    \"" << input << L"\"" << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Invalid number of parameters in this visitor data (should be 5): " << parameters.size() << "\n";
		std::wcerr << L"    \"" << input << L"\"" << std::endl;
	}

	return false;
}


// =================================================================================================================
// == Location =====================================================================================================
// =================================================================================================================


void Group::UpdateLocation(int x, int y)
{
	Behaviour.location.x = x;
	Behaviour.location.y = y;

	if (x == Constants::kLocationExitedPark && y == Constants::kLocationExitedPark)
	{
		SetStatusForAllVisitors(GroupParkStatus::Exited, VisitorParkStatus::Exited);
	}
}


void Group::UpdateTravellingLocation(void)
{
	double dx = abs((double)Behaviour.travelling.from.x - (double)Behaviour.travelling.to.x);
	double dy = abs((double)Behaviour.travelling.from.y - (double)Behaviour.travelling.to.y);

	double d = sqrtf(dx * dx + dy * dy);

	double deltad = d * (((double)Behaviour.travelling.minutesStart - (double)Behaviour.travelling.minutesLeft) / (double)Behaviour.travelling.minutesStart);

	int newx = static_cast<int>((deltad / d) * ((double)Behaviour.travelling.to.x - (double)Behaviour.travelling.from.x) + (double)Behaviour.travelling.from.x);
	int newy = static_cast<int>((deltad / d) * ((double)Behaviour.travelling.to.y - (double)Behaviour.travelling.from.y) + (double)Behaviour.travelling.from.y);

	Behaviour.location.x = newx;
	Behaviour.location.y = newy;
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


std::wstring Group::GetVisitorAgesString()
{
	std::wstring output = L"";

	for (int v = 0; v < Visitors.size(); v++)
	{
		output += Visitors[v].Configuration.GetAgeAsString() + L" ";
	}

	return output;
}


void Group::SetStatForAllVisitors(GroupVisitorStat stat_id)
{
	for (int v = 0; v < Visitors.size(); v++)
	{
		switch (stat_id)
		{
			case GroupVisitorStat::NoRideAvailable:
				Visitors[v].Rides.noRideAvailable++;
				break;
			case GroupVisitorStat::RideShutdown:
				Visitors[v].Rides.rideShutdown++;
				break;
			case GroupVisitorStat::WaitTimeTooLong:
				Visitors[v].Rides.waitTimeTooLong++;
				break;
			case GroupVisitorStat::NoFastPassRideForMe:
				Visitors[v].Rides.noFastPassRideForMe++;
				break;
			case GroupVisitorStat::TimeSpentRiding:
				if (Visitors[v].ParkStatus == VisitorParkStatus::Riding)
				{
					Visitors[v].TimeSpent.riding++;
				}
				break;
			case GroupVisitorStat::TimeSpentIdle:
				Visitors[v].TimeSpent.idle++;
				break;
			case GroupVisitorStat::TimeSpentQueuing:
				Visitors[v].TimeSpent.queuing++;
				break;
			case GroupVisitorStat::TimeSpentQueuingFood:
				Visitors[v].TimeSpent.queuingFood++;
				break;
			case GroupVisitorStat::TimeSpentTravelling:
				Visitors[v].TimeSpent.travelling++;
				break;
			case GroupVisitorStat::TimeSpentTravellingFood:
				Visitors[v].TimeSpent.travellingFood++;
				break;
			case GroupVisitorStat::TimeSpentWaiting:
				Visitors[v].TimeSpent.waiting++;
				break;
			case GroupVisitorStat::TimeSpentEating:
				Visitors[v].TimeSpent.eating++;
				break;
			case GroupVisitorStat::NooneCanRideInGroup:
				Visitors[v].Rides.nooneCanRideInGroup++;
				break;
			case GroupVisitorStat::EateryQueueTooLong:
				Visitors[v].Rides.eateryQueueTooLong++;
		}
	}
}


void Group::SaveMinuteStats()
{
	GroupMinuteDataVI m;

	m.position.x = Behaviour.location.x;
	m.position.y = Behaviour.location.y;

	m.parkStatus = Behaviour.parkStatus;

	switch (Behaviour.parkStatus)
	{
	case GroupParkStatus::OnWay:
		m.parkStatusInt = 0;
		break;
	case GroupParkStatus::AtEntrance:
		m.parkStatusInt = 1;
		break;
	case GroupParkStatus::Idle:
		m.parkStatusInt = 2;
		break;
	case GroupParkStatus::Riding:
		m.parkStatusInt = 3;
		break;
	case GroupParkStatus::Queuing:
		m.parkStatusInt = 4;
		break;
	case GroupParkStatus::QueuingFastPass:
		m.parkStatusInt = 5;
		break;
	case GroupParkStatus::TravellingRide:
		m.parkStatusInt = 6;
		break;
	case GroupParkStatus::Waiting:
		m.parkStatusInt = 7;
		break;
	case GroupParkStatus::TravellingFood:
		m.parkStatusInt = 8;
		break;
	case GroupParkStatus::Eating:
		m.parkStatusInt = 9;
		break;
	case GroupParkStatus::Exited:
		m.parkStatusInt = 10;
		break;
	}

	Statistics.minuteLog.push_back(m);
}


std::wstring Group::GetLocationByMinute(int minute)
{
	return L"," + std::to_wstring(minute) + L"," + std::to_wstring(Statistics.minuteLog[minute].position.x) + L"," + std::to_wstring(Statistics.minuteLog[minute].position.y) + L"," + std::to_wstring(Statistics.minuteLog[minute].parkStatusInt);
}