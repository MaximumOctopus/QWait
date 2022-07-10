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

	float random = (float)rand() / RAND_MAX;

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
}


void Group::BuildFamily(float random)
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
}


void Group::BuildAdultCouple(float random)
{
	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType((float)rand() / RAND_MAX));

	Visitors.push_back(Visitor(vtc));
	Visitors.push_back(Visitor(vtc));
}


void Group::BuildAdultGroup(float random)
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
		Visitors.push_back(Visitor(GetBaseVisitorConfiguration(GetType(rand() % 6))));
	}
}


void Group::BuildSingle(float random)
{
	Visitors.push_back(Visitor(GetBaseVisitorConfiguration(GetType(rand() % 7))));
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


VisitorType Group::GetType(float random)
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
	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType(rand() % 5));

	vtc.age = AgeGroup::Adult;

	return vtc;
}


NewVisitorConfiguration Group::GetVisitorGenericChild()
{
	NewVisitorConfiguration vtc = GetBaseVisitorConfiguration(GetType(rand() % 5));

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
		vtc.maxWaitTime = 240.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEnthusiast];

		break;

	case VisitorType::Fan:
		vtc.maxWaitTime = 120.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeFan];

		break;

	case VisitorType::Tourist:
		vtc.maxWaitTime = 90.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeTourist];

		break;

	case VisitorType::Activity:
		vtc.maxWaitTime = 70.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeActivity];

		break;

	case VisitorType::Passholder:
		vtc.maxWaitTime = 60.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypePassholder];

		break;

	case VisitorType::EPassHolder:
		vtc.maxWaitTime = 60.0f;
		vtc.preference = Constants::VisitorTypesRidePreference[Constants::VisitorTypeEPassHolder];

		break;

	case VisitorType::TikToker:
		vtc.maxWaitTime = 45.0f;
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
	float mwt = 0;

	for (int v = 0; v < Visitors.size(); v++)
	{
		mwt += Visitors[v].Rides.maxWaitTime;
	}

	return static_cast<int>((mwt / (float)Visitors.size()) * 1.05f);
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


void Group::SetQueuing(int visitor, float queue_length)
{
	Visitors[visitor].SetQueuingLengthStat(queue_length);
}


void Group::SetQueuingFastPass(int visitor, float queue_length)
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

	SetTravellingMinutesLeft(minutes_left);

	for (int v = 0; v < Visitors.size(); v++)
	{
		Visitors[v].Rides.distanceTravelled += distance;
	}

	Behaviour.travelling.to.x = location.x;
	Behaviour.travelling.to.y = location.y;
}


void Group::SetTravellingMinutesLeft(int minutes)
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

	SetStatusForAllVisitors(GroupParkStatus::Travelling, VisitorParkStatus::Travelling);
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


bool Group::CreateVisitorFromFileData(const std::string input)
{
	std::stringstream visitorinput(input);
	std::vector<std::string> parameters;
	std::string parameter;

	while (std::getline(visitorinput, parameter, ','))
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
			std::cerr << "There appears to be some dodgy data in this input row:" << "\n";
			std::cerr << "    \"" << input << "\"" << std::endl;
		}
	}
	else
	{
		std::cerr << "Invalid number of parameters in this visitor data (should be 5): " << parameters.size() << "\n";
		std::cerr << "    \"" << input << "\"" << std::endl;
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
	float dx = abs((float)Behaviour.travelling.from.x - (float)Behaviour.travelling.to.x);
	float dy = abs((float)Behaviour.travelling.from.y - (float)Behaviour.travelling.to.y);

	float d = sqrtf(dx * dx + dy * dy);

	float deltad = d * (((float)Behaviour.travelling.minutesStart - (float)Behaviour.travelling.minutesLeft) / (float)Behaviour.travelling.minutesStart);

	int newx = static_cast<int>((deltad / d) * ((float)Behaviour.travelling.to.x - (float)Behaviour.travelling.from.x) + (float)Behaviour.travelling.from.x);
	int newy = static_cast<int>((deltad / d) * ((float)Behaviour.travelling.to.y - (float)Behaviour.travelling.from.y) + (float)Behaviour.travelling.from.y);

	Behaviour.location.x = newx;
	Behaviour.location.y = newy;
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


std::string Group::GetVisitorAgesString()
{
	std::string output = "";

	for (int v = 0; v < Visitors.size(); v++)
	{
		output += Visitors[v].Configuration.GetAgeAsString() + " ";
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
			case GroupVisitorStat::TimeSpentTravelling:
				Visitors[v].TimeSpent.travelling++;
				break;
			case GroupVisitorStat::TimeSpentWaiting:
				Visitors[v].TimeSpent.waiting++;
				break;
			case GroupVisitorStat::NooneCanRideInGroup:
				Visitors[v].Rides.nooneCanRideInGroup++;
				break;
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
	case GroupParkStatus::Travelling:
		m.parkStatusInt = 6;
		break;
	case GroupParkStatus::Waiting:
		m.parkStatusInt = 7;
		break;
	case GroupParkStatus::Exited:
		m.parkStatusInt = 8;
		break;
	}

	Statistics.minuteLog.push_back(m);
}


std::string Group::GetLocationByMinute(int minute)
{
	return "," + std::to_string(minute) + "," + std::to_string(Statistics.minuteLog[minute].position.x) + "," + std::to_string(Statistics.minuteLog[minute].position.y) + "," + std::to_string(Statistics.minuteLog[minute].parkStatusInt);
}