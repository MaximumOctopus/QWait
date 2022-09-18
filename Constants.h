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


namespace Constants
{
	static const std::wstring QWVersion = L"0.7";
	static const std::wstring QWDate = L"August 31st 2022";

	// ===========================================================================================================================

	static const int TemplateMaximum = 6;

	// ===========================================================================================================================

	const static int MaxRideCount = 30;

	// ===========================================================================================================================

	const static double ArriveAt10 = 10.0;
	const static double ArriveAt11 = 21.0;
	const static double ArriveAt12 = 16.1;
	const static double ArriveAt13 = 15.4;
	const static double ArriveAt14 = 14.1;
	const static double ArriveAt15 = 10.0;
	const static double ArriveAt16 = 9.0;
	const static double ArriveAt17 = 1.1;
	const static double ArriveAt18 = 1.7;
	const static double ArriveAt19 = 1.0;
	const static double ArriveAt20 = 0.6;
	const static double ArriveAt21 = 0.0;

	const static int AvailableVisitorTypes = 7;
	const static int AvailableGroupTypes = 4;

	const static int VisitorTypeEnthusiast = 0;
	const static int VisitorTypeFan = 1;
	const static int VisitorTypeTourist = 2;
	const static int VisitorTypeActivity = 3;
	const static int VisitorTypePassholder = 4;
	const static int VisitorTypeEPassHolder = 5;
	const static int VisitorTypeTikToker = 6;
	const static int VisitorTypeVIP = 7;

	const static std::wstring GroupNames[AvailableGroupTypes]  = { L"Family", L"Adult Couple", L"Adult Group", L"Single" };

	const static int PercentageStayingOnSiteStartingEarly = 10; 

	const static double VisitorTypesRidePreference[AvailableVisitorTypes]       = { 0.1, 0.2, 0.4, 0.8, 0.5, 0.5, 0.4 };
	const static double VisitorTypesRidePreferenceStdDev[AvailableVisitorTypes] = { 2.5, 2.5, 5.5, 2.0, 2.0, 2.0, 2.0 };

	static const int WalkSpeedMetresPerMinute = 70;

	// ===========================================================================================================================

	const static int FastPassTicketsAllowes[3] = { 0, 3, 3 }; // tickets allowed per visitor per different fastpass modes

	const static double FastPassHourlyPercent = 75.0; // these two must add to 100 :)
	const static double StandardHourlyPercent = 25.0; //

	// ===========================================================================================================================

	const static int ParkStatusOnWay = 0; // on way to the park
	const static int ParkStatusAtEntrance = 1; // queuing at the entrance
	const static int ParkStatusIdle = 2; // in the park, doing nothing
	const static int ParkStatusRiding = 3; // currently riding a ride/attraction
	const static int ParkStatusQueuing = 4; // queuing for a ride
	const static int ParkStatusQueuingFastPass = 5; // queuing for a ride with FastPass
	const static int ParkStatusQueuingFoodDrink = 6;
	const static int ParkStatusTravelling = 7; // travelling to a ride
	const static int ParkStatusTravellingFoodDrink = 8; // travelling to an eatery
	const static int ParkStatusEating = 9;
	const static int ParkStatusWaiting = 10; // visitor is doing something which causes to be busy doing something
	const static int ParkStatusWaitingForOthersInParty = 11;
	const static int ParkStatusExited = 12; // left the park

	const static int kNoCurrentRide = -1;
	const static int kNoDestinationRide = -1;
	const static int kNoDestinationEatery = -1;
	const static int kNoFastPass = -1;
	const static int kNoSelectedRide = -1;
	const static int kNoSelectedEatery = -1;
	const static int kNoSelectedType = -1;
	const static int kNotValidVisitor = -1;
	const static int kNotValidGroup = -1;
	const static int kLocationExitedPark = -1;

	const static int VisitorsInPark = 10;
}