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


#pragma once


#include <string>


namespace Constants
{
	static const std::string QWVersion = "0.4";
	static const std::string QWDate = "April 14th 2022";

	// ===========================================================================================================================

	const static int MaxRideCount = 30;

	const static int TemplateGenericTest = 0;
	const static int TemplateWDWAnimalKingdom = 1;
	const static int TemplateWDWEpcot = 2;
	const static int TemplateWDWHollywoodStudios = 3;
	const static int TemplateWDWMagicKingdom = 4;
	const static int TemplateArcade = 5;
	const static int TemplateMaximum = TemplateArcade;

	// ===========================================================================================================================

	const static float ArriveAt10 = 10.0f;
	const static float ArriveAt11 = 21.0f;
	const static float ArriveAt12 = 16.1f;
	const static float ArriveAt13 = 14.4f;
	const static float ArriveAt14 = 14.1f;
	const static float ArriveAt15 = 10.0f;
	const static float ArriveAt16 = 10.0f;
	const static float ArriveAt17 = 1.1f;
	const static float ArriveAt18 = 1.7f;
	const static float ArriveAt19 = 1.0f;
	const static float ArriveAt20 = 0.6f;
	const static float ArriveAt21 = 0.0f;

	const static int AvailableVisitorTypes = 7;

	const static int VisitorTypeEnthusiast = 0;
	const static int VisitorTypeFan = 1;
	const static int VisitorTypeTourist = 2;
	const static int VisitorTypeActivity = 3;
	const static int VisitorTypePassholder = 4;
	const static int VisitorTypeEPassHolder = 5;
	const static int VisitorTypeTikToker = 6;
	const static int VisitorTypeVIP = 7;

	const static std::string TypeNames[AvailableVisitorTypes] = { "Enthusiast", "Fan", "Tourist", "Activity", "Passholder", "Passholder (E)", "Tiktoker", "VIP" };

	const static int PercentageStayingOnSiteStartingEarly = 10; 

	const static float VisitorTypesRidePreference[AvailableVisitorTypes]       = { 0.1f, 0.2f, 0.4f, 0.8f, 0.5f, 0.5f, 0.4f };
	const static float VisitorTypesRidePreferenceStdDev[AvailableVisitorTypes] = { 2.5f, 2.5f, 5.5f, 2.0f, 2.0f, 2.0f, 2.0f };

	// ===========================================================================================================================

	const static int FastPassModeNone = 0;
	const static int FastPassModePlus = 1;
	const static int FastPassModePlusNoStayBenefit = 2; // visitors staying onsite do not get first pick on fastpass tickets

	const static std::string FastPassModeNames[3] = { "None.", "FastPass+", "FastPass+ (no stay benefits) "};
	const static int FastPassTicketsAllowes[3] = { 0, 3, 3 }; // tickets allowed per visitor per different fastpass modes

	const static float FastPassHourlyPercent = 75.0f; // these two must add to 100 :)
	const static float StandardHourlyPercent = 25.0f; //

	// ===========================================================================================================================

	const static int ParkStatusOnWay = 0; // on way to the park
	const static int ParkStatusAtEntrance = 1; // queuing at the entrance
	const static int ParkStatusIdle = 2; // in the park, doing nothing
	const static int ParkStatusRiding = 3; // currently riding a ride/attraction
	const static int ParkStatusQueuing = 4; // queuing for a ride
	const static int ParkStatusQueuingFastPass = 5; // queuing for a ride with FastPass
	const static int ParkStatusTravelling = 6; // travelling to a ride
	const static int ParkStatusWaiting = 7; // visitor is doing something which causes to be busy doing something
	const static int ParkStatusExited = 8; // left the park

	const static int VisitorsInPark = 9;
}