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


#include <vector>

#include "Constants.h"


enum class RideType { Continuous = 0, Show = 1 };

enum class RideExitType { None, Short, Long, Shop, LargeShop };

//                                            visitors staying onsite do not get first pick on fastpass tickets
enum class FastPassType { None = 0, Plus = 1, PlusNoStayBenefit = 2 };

enum class VisitorType {
	Enthusiast = 0, Fan = 1, Tourist = 2, Activity = 3,
	Passholder = 4, EPassHolder = 5, TikToker = 6, VIP = 7
};

enum class ParkTemplate {
	Test = 0, WDWAnimalKingdom = 1, WDWEpcot = 2, WDWHollywoodStudios = 3,
	WDWMagicKingdom = 4, Arcade = 5, AltonTowers = 6
};


namespace QWaitTypes
{
	struct Coords {
		int x = 0;
		int y = 0;
	};

	struct Time {
		int hours = 0;
		int minutes = 0;
	};

	struct FastPass {
		int type = 0;		// 1, 2, or 3
		int ride = 0;

		Time ticket = { 0, 0 };
	};

	struct GetRide {
		int ride = 0;
		int fastPassTicket = 0;
	};

	// represents a group (or part therof) in a ride queue.
	// group id in the Groups array, members: one visitor array index per member of the group in the queue

	struct Riders
	{
		int group = Constants::kNotValidGroup;	// group id riding

		std::vector<int> memberID;
	};


	int RideExitTypeToInt(RideExitType);
	RideExitType IntToRideExitType(int);
	std::wstring RideExitTypeToString(RideExitType);

	FastPassType IntToFastPassMode(int);
	std::wstring FastPassModeToString(FastPassType);

	VisitorType IntToVisitorType(int);
	std::wstring VisitorTypeToString(VisitorType);
	std::wstring VisitorTypeToString(int);
	int VisitorTypeToInt(VisitorType);

	int ParkTemplateToInt(ParkTemplate);
	ParkTemplate IntToParkTemplate(int);
}