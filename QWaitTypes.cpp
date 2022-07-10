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


#include "QWaitTypes.h">


namespace QWaitTypes
{
	int QWaitTypes::RideExitTypeToInt(RideExitType ride_exit_type)
	{
		switch (ride_exit_type)
		{
		case RideExitType::None:
			return 0;
			break;
		case RideExitType::Short:
			return 1;
			break;
		case RideExitType::Long:
			return 2;
			break;
		case RideExitType::Shop:
			return 3;
			break;
		case RideExitType::LargeShop:
			return 4;
			break;
		}

		return 0;
	}


	RideExitType QWaitTypes::IntToRideExitType(int ride_exit_type)
	{
		switch (ride_exit_type)
		{
		case 0:
			return RideExitType::None;
			break;
		case 1:
			return RideExitType::Short;
			break;
		case 2:
			return RideExitType::Long;
			break;
		case 3:
			return RideExitType::Shop;
			break;
		case 4:
			return RideExitType::LargeShop;
			break;
		}

		return RideExitType::None;
	}


	std::string QWaitTypes::RideExitTypeToString(RideExitType ride_exit_type)
	{
		switch (ride_exit_type)
		{
		case RideExitType::None:
			return "None";
			break;
		case RideExitType::Short:
			return "Short";
			break;
		case RideExitType::Long:
			return "Long";
			break;
		case RideExitType::Shop:
			return "Shop";
			break;
		case RideExitType::LargeShop:
			return "Large shop";
			break;
		}

		return "Unknown";
	}


	FastPassType QWaitTypes::IntToFastPassMode(int fast_pass_mode)
	{
		switch (fast_pass_mode)
		{
		case 0:
			return FastPassType::None;
			break;
		case 1:
			return FastPassType::Plus;
			break;
		case 2:
			return FastPassType::PlusNoStayBenefit;
			break;
		}

		return FastPassType::None;
	}


	std::string QWaitTypes::FastPassModeToString(FastPassType fast_pass_mode)
	{
		switch (fast_pass_mode)
		{
		case FastPassType::None:
			return "None";
			break;
		case FastPassType::Plus:
			return "FastPass+";
			break;
		case FastPassType::PlusNoStayBenefit:
			return "FastPass+ (no stay benefits)";
			break;
		}

		return "Unknown";
	}


	VisitorType QWaitTypes::IntToVisitorType(int visitor_type)
	{
		switch (visitor_type)
		{
		case 0:
			return VisitorType::Enthusiast;
		case 1:
			return VisitorType::Fan;
		case 2:
			return VisitorType::Tourist;
		case 3:
			return VisitorType::Activity;
		case 4:
			return VisitorType::Passholder;
		case 5:
			return VisitorType::EPassHolder;
		case 6:
			return VisitorType::TikToker;
		case 7:
			return VisitorType::VIP;
		}

		return VisitorType::Enthusiast;
	}


	std::string QWaitTypes::VisitorTypeToString(VisitorType visitor_type)
	{
		switch (visitor_type)
		{
		case VisitorType::Enthusiast:
			return "Enthusiast";
			break;
		case VisitorType::Fan:
			return "Fan";
			break;
		case VisitorType::Tourist:
			return "Tourist";
			break;
		case VisitorType::Activity:
			return "Activity";
			break;
		case VisitorType::Passholder:
			return "Passholder";
			break;
		case VisitorType::EPassHolder:
			return "Passholder (E)";
			break;
		case VisitorType::TikToker:
			return "Tiktoker";
			break;
		case VisitorType::VIP:
			return "VIP";
			break;
		}
		
		return "Unknown";
	}


	std::string QWaitTypes::VisitorTypeToString(int visitor_type)
	{
		switch (visitor_type)
		{
		case 0:
			return "Enthusiast";
			break;
		case 1:
			return "Fan";
			break;
		case 2:
			return "Tourist";
			break;
		case 3:
			return "Activity";
			break;
		case 4:
			return "Passholder";
			break;
		case 5:
			return "Passholder (E)";
			break;
		case 6:
			return "Tiktoker";
			break;
		case 7:
			return "VIP";
			break;
		}

		return "Unknown";
	}


	int QWaitTypes::VisitorTypeToInt(VisitorType visitor_type)
	{
		switch (visitor_type)
		{
		case VisitorType::Enthusiast:
			return 0;
			break;
		case VisitorType::Fan:
			return 1;
			break;
		case VisitorType::Tourist:
			return 2;
			break;
		case VisitorType::Activity:
			return 3;
			break;
		case VisitorType::Passholder:
			return 4;
			break;
		case VisitorType::EPassHolder:
			return 5;
			break;
		case VisitorType::TikToker:
			return 6;
			break;
		case VisitorType::VIP:
			return 7;
			break;
		}

		return 0;
	}


	int QWaitTypes::ParkTemplateToInt(ParkTemplate park_template)
	{
		switch (park_template)
		{
		case ParkTemplate::Test:
			return 0;
		case ParkTemplate::WDWAnimalKingdom:
			return 1;
		case ParkTemplate::WDWEpcot:
			return 2;
		case ParkTemplate::WDWHollywoodStudios:
			return 3;
		case ParkTemplate::WDWMagicKingdom:
			return 4;
		case ParkTemplate::Arcade:
			return 5;
		case ParkTemplate::AltonTowers:
			return 6;
		}

		return 0;
	}


	ParkTemplate QWaitTypes::IntToParkTemplate(int park_template)
	{
		switch (park_template)
		{

		case 0:
			return ParkTemplate::Test;
		case 1:
			return ParkTemplate::WDWAnimalKingdom;
		case 2:
			return ParkTemplate::WDWEpcot;
		case 3:
			return ParkTemplate::WDWHollywoodStudios;
		case 4:
			return ParkTemplate::WDWMagicKingdom;
		case 5:
			return ParkTemplate::Arcade;
		case 6:
			return ParkTemplate::AltonTowers;
		}

		return ParkTemplate::WDWAnimalKingdom;
	}
}