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


// Sources:
//   https://wdwthemeparks.com/hourly-capacity-numbers/
//   https://www.wdwinfo.com/wdwinfo/ridelength.htm
//   https://crooksinwdw.wordpress.com/2013/12/14/theoreticaloperational-hourly-ride-capacity-at-wdw/
//   https://eu.azcentral.com/story/travel/destinations/california/2019/08/16/disneyland-ride-capacity-how-long-will-you-have-wait-line/1927184001/
//   https://magicguides.com/disney-world-statistics/
//
//   https://www.mousehacking.com/blog/best-animal-kingdom-rides
//   https://www.mousehacking.com/blog/every-walt-disney-world-ride-ranked
//   https://magicguides.com/best-epcot-attractions/
//   https://www.mousehacking.com/blog/disney-world-height-requirements
// 
//   https://www.disneytouristblog.com/paid-lightning-lanes-genie-replace-free-fastpass-disney-world/
//
//	 https://www.wdwinfo.com/wdwinfo/fastpass.htm
//   https://forums.wdwmagic.com/threads/percentage-of-fps-allocated-for-rides-wdw.955357/
//
//   https://towersstreet.com/theme-park/ride/
//   https://www.themeparkjames.co.uk/theme-parks/europe/uk/alton-towers/
//   https://www.youtube.com/watch?v=cVUlHjQximM


#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include "Configuration.h"
#include "Constants.h"
#include "Ini.h"
#include "ParkController.h"
#include "Ride.h"
#include "Utility.h"


extern Configuration* GConfiguration;


ParkController* GParkController;


// results in a list where Rides[0] is the high popularity (lowest value)
bool sortByPopularity(const Ride& r1, const Ride& r2)
{
	return r1.RideOperation.Popularity < r2.RideOperation.Popularity;
}


// results in a list where Rides[0] is the closest (lowest value)
bool sortByDistance(const DistanceInfo& r1, const DistanceInfo& r2)
{
	return r1.distanceTo < r2.distanceTo;
}


ParkController::ParkController(bool show_config, const std::string ride_template_file)
{
	entrance.ParkName = "None.";

	ShowOutput = show_config;

	FastPassMode = GConfiguration->FastPassMode;

	entrance.position.x = 0;
	entrance.position.y = 0;

	Minutes    = 0;

	TheoreticalHourThroughputTotal = 0;
	TheoreticalHourThroughputTotalDay = 0;

	for (int x = 0; x < Constants::MaxRideCount; x++)
	{
		for (int y = 0; y < Constants::MaxRideCount; y++)
		{
			WalkTimeCache[x][y]     = 0;
			WalkDistanceCache[x][y] = 0;
		}
	}
}


void ParkController::SetEntrance(const std::string name, int x, int y, int fast_pass_type, int average_visitors, int adult, int adult_advance, int child, int child_advance, int babies, int babies_advance)
{
	entrance.ParkName = name;

	entrance.position.x = x;
	entrance.position.y = y;
	entrance.throughPutMinute = 100;

	entrance.fastPassType = fast_pass_type;

	entrance.averageVisitors = average_visitors;

	entrance.Prices.Adult = adult;
	entrance.Prices.AdultAdvance = adult_advance;
	entrance.Prices.Children = child;
	entrance.Prices.ChildrenAdvance = child_advance;
	entrance.Prices.Babies = babies;
	entrance.Prices.BabiesAdvance = babies_advance;
}


void ParkController::AddNewRideContinuous(const std::string name, int ride_length, RideExitType ride_exit_type, int hourly_throughput, int popularity, int x, int y, int is_fast_pass, int fp_reserve_percent, bool AdultValid, bool ChildValid, bool BabyValid)
{
	if (FastPassMode == FastPassType::None)
	{ 
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { GConfiguration->ParkOpenTime, 0 };
	QWaitTypes::Time close = { GConfiguration->ParkCloseTime, 0 };

	int ophours = GConfiguration->ParkOpenHours;

	Ride r(RideType::Continuous, name, ride_length, ride_exit_type, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours, AdultValid, ChildValid);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void ParkController::AddNewRideShow(const std::string name, int ride_length, RideExitType ride_exit_type, int StartTimeMinutes, int hourly_throughput, int show_capacity, int popularity, int x, int y, int IsFastPass, int fp_reserve_percent, bool AdultValid, bool ChildValid, bool BabyValid)
{
	if (FastPassMode == FastPassType::None)
	{
		IsFastPass = 0;
	}

	QWaitTypes::Time open = { GConfiguration->ParkOpenTime, 0 };
	QWaitTypes::Time close = { GConfiguration->ParkCloseTime, 0 };

	int ophours = GConfiguration->ParkOpenHours;

	Ride r(RideType::Show, name, ride_length, ride_exit_type, hourly_throughput, popularity, x, y, IsFastPass, fp_reserve_percent, open, close, ophours, AdultValid, ChildValid);

	r.RideOperation.ShowStartTime = StartTimeMinutes;

	r.ConfigureShowThroughput(show_capacity, fp_reserve_percent);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void ParkController::AddNewRideContinuousWithHours(const std::string name, int ride_length, RideExitType ride_exit_type, int hourly_throughput, int popularity, int open_hour, int open_minute, int close_hour, int close_minute, int x, int y, int is_fast_pass, int fp_reserve_percent, bool AdultValid, bool ChildValid, bool BabyValid)
{
	if (FastPassMode == FastPassType::None)
	{
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { open_hour, open_minute };
	QWaitTypes::Time close = { close_hour, close_minute };

	int ophours = Utility::Duration(open, close);

	Ride r(RideType::Continuous, name, ride_length, ride_exit_type, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours, AdultValid, ChildValid);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void ParkController::AddNewRideShowWithHours(const std::string name, int ride_length, RideExitType ride_exit_type, int start_time_minutes, int hourly_throughput, int show_capacity, int popularity, int open_hour, int open_minute, int close_hour, int close_minute, int x, int y, int is_fast_pass, int fp_reserve_percent, bool AdultValid, bool ChildValid, bool BabyValid)
{
	if (FastPassMode == FastPassType::None)
	{
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { open_hour, open_minute };
	QWaitTypes::Time close = { close_hour, close_minute };

	int ophours = Utility::Duration(open, close);

	Ride r(RideType::Show, name, ride_length, ride_exit_type, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours, AdultValid, ChildValid);

	r.RideOperation.ShowStartTime = start_time_minutes;

	r.ConfigureShowThroughput(show_capacity, fp_reserve_percent);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void ParkController::BuildRides(ParkTemplate park_template, const std::string file_name)
{
	if (file_name != "")
	{
		BuildFromTemplate(file_name);
	}
	else
	{
		BuildFromTemplate(park_template);
	}

	std::sort(Rides.begin(), Rides.end(), sortByPopularity);

	BuildDistanceCache();

	BuildClosestCache();

	BuildSelectionChoiceCache();

	BuildFastPassList();

	ShowConfig();
}


void ParkController::AddToLog(const std::string log_entry)
{
	Log.push_back(log_entry);
}


int ParkController::GetDistanceBetweenInMinutes(int ride1, int ride2)
{
	int Location1 = ride1;
	int Location2 = ride2;
	
	if (ride1 == Constants::kNoSelectedRide)
	{
		Location1 = Rides.size();
	}

	if (ride2 == Constants::kNoSelectedRide)
	{
		Location2 = Rides.size();
	}

	return WalkTimeCache[Location1][Location2];
}


int ParkController::GetDistanceBetweenInMetres(int ride1, int ride2)
{
	int Location1 = ride1;
	int Location2 = ride2;

	if (ride1 == Constants::kNoSelectedRide)
	{
		Location1 = Rides.size();
	}

	if (ride2 == Constants::kNoSelectedRide)
	{
		Location2 = Rides.size();
	}

	return WalkDistanceCache[Location1][Location2];
}


void ParkController::BuildDistanceCache()
{
	static const int kWalkSpeedMetresPerMinute = 70;

	for (int r1 = 0; r1 <= Rides.size(); r1++)
	{
		for (int r2 = 0; r2 <= Rides.size(); r2++)
		{
			int X1;
			int Y1;

			int X2;
			int Y2;

			if (r1 == Rides.size())
			{
				X1 = entrance.position.x;
				Y1 = entrance.position.y;
			}
			else
			{
				X1 = Rides[r1].RideOperation.position.x;
				Y1 = Rides[r1].RideOperation.position.y;
			}

			if (r2 == Rides.size())
			{
				X2 = entrance.position.x;
				Y2 = entrance.position.y;
			}
			else
			{
				X2 = Rides[r2].RideOperation.position.x;
				Y2 = Rides[r2].RideOperation.position.y;
			}

			int dx = abs(X1 - X2);
			int dy = abs(Y1 - Y2);

			if (r1 == r2)
			{
				WalkTimeCache[r1][r2] = 0;
				WalkDistanceCache[r1][r2] = 0;
			}
			else
			{
				WalkTimeCache[r1][r2]     = static_cast<int>(sqrt((double)dx * dx + dy * dy) / kWalkSpeedMetresPerMinute) + 3;
				WalkDistanceCache[r1][r2] = static_cast<int>(sqrt((double)dx * dx + dy * dy));
			}
		}
	}
}


void ParkController::BuildClosestCache()
{
	std::vector<DistanceInfo> Distance;

	for (int ride = 0; ride < Rides.size(); ride++)
	{
		Distance.clear();

		for (int to = 0; to < Rides.size(); to++)
		{
			if (ride != to)
			{
				int X1 = Rides[ride].RideOperation.position.x;
				int Y1 = Rides[ride].RideOperation.position.y;

				int X2 = Rides[to].RideOperation.position.x;
				int Y2 = Rides[to].RideOperation.position.y;

				int dx = abs(X1 - X2);
				int dy = abs(Y1 - Y2);

				DistanceInfo di;

				di.distanceTo = static_cast<int>(sqrt((double)dx * dx + dy * dy));
				di.ride = to;

				Distance.push_back(di);
			}
		}

		std::sort(Distance.begin(), Distance.end(), sortByDistance);

		for (int c = 0; c < kClosestCacheSize; c++)
		{
			Rides[ride].ClosestCache[c] = Distance[c].ride;
		}
	}
}


void ParkController::BuildSelectionChoiceCache()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	int SelectedRide = 0;

	for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
	{
		srand(time(NULL));

		int count = 0;

		while (count < 200)
		{
			std::normal_distribution <double> d(static_cast<int>(GParkController->Rides.size() * Constants::VisitorTypesRidePreference[t]), Constants::VisitorTypesRidePreferenceStdDev[t]);

			SelectedRide = d(gen);

			if (SelectedRide < 0)
			{
				SelectedRide = abs(SelectedRide);
			}

			if (SelectedRide >= 0 && SelectedRide <= GParkController->Rides.size() - 1)
			{
				SelectionChoiceCache[t][count] = SelectedRide;

				count++;
			}
		}
	}
}


// add this ride to the correct fastpass list type 1, type 2, or type 3
// already ordered by popularity
void ParkController::BuildFastPassList()
{
	for (int r = 0; r < Rides.size(); r++)
	{
		if (Rides[r].FastPass.mode != 0)
		{
			FastPassTypes[Rides[r].FastPass.mode - 1].push_back(r);
		}
	}
}


size_t ParkController::EntranceQueueSize()
{
	return EntranceQueue.size();
}


void ParkController::AddToEntranceQueue(int group_id)
{
	EntranceQueue.push_back(group_id);
}


int ParkController::RemoveFromEntranceQueue()
{
	if (EntranceQueue.size() != 0)
	{
		int groupID = EntranceQueue[0];

		EntranceQueue.erase(EntranceQueue.begin());

		return groupID;
	}

	return Constants::kNotValidGroup;
}


int ParkController::GetMinuteCount()
{
	return Minutes - 1;
}


void ParkController::UpdateThroughput()
{
	TheoreticalHourThroughputTotalDay = TheoreticalHourThroughputTotal * GConfiguration->ParkOpenHours;
}


void ParkController::ShowConfig()
{
	if (ShowOutput)
	{
		std::cout << "Park Name                     : " << entrance.ParkName << "\n";
		std::cout << "Number of rides in park       : " << Rides.size() << "\n";
		std::cout << "Theoretical hourly throughput : " << TheoreticalHourThroughputTotal << "\n";
		std::cout << "Theoretical daily throughput  : " << TheoreticalHourThroughputTotalDay << "\n";
		std::cout << std::endl;
	}
}


void ParkController::BuildFromTemplate(ParkTemplate park_template)
{
	switch (park_template)\
	{
	case ParkTemplate::Test:

		SetEntrance("Super Cat Park", 0, 0, 0, 10000,
			8, 8, 5, 5, 0, 0);

		AddNewRideShow("Cats throughout the ages", 30,             RideExitType::Short, 60, 2000, 2000,  1, -200,  50, 0, 0, true, true, false);
		AddNewRideShow("Idea of a certain cat", 10,                RideExitType::Short, 15, 1000,  250, 12,    3, 300, 0, 0, true, true, false);

		AddNewRideContinuous("Katzen-Kogl", 4,                     RideExitType::Short, 1800,  4, -200, 150, 0, 0, true, true, false);
		AddNewRideContinuous("Le chat blanc", 4,                   RideExitType::Short, 1800,  2,  200, 150, 0, 0, true, true, false);
		AddNewRideContinuous("Raminou", 4,                         RideExitType::Short, 1800, 10, -200, 250, 0, 0, true, true, false);
		AddNewRideContinuous("White cat playing with a string", 4, RideExitType::Short, 1800,  7,  200, 250, 0, 0, true, true, false);
		AddNewRideContinuous("Rutherford Freeman Maxwell", 4,      RideExitType::Short, 1800,  8,  200,  50, 0, 0, true, true, false);
		AddNewRideContinuous("Siamese adventure", 4,               RideExitType::Short, 1800,  9,    0, 100, 0, 0, true, true, false);
		AddNewRideContinuous("Fluffy cat surprise", 4,             RideExitType::Short, 1800,  5,    0, 200, 0, 0, true, true, false);
		AddNewRideContinuous("Meow meow meow!", 4,                 RideExitType::Short, 1800,  6,    0,  50, 0, 0, true, true, false);

		break;

	case ParkTemplate::WDWAnimalKingdom:

		SetEntrance("Animal Kingdom", 0, 0, 1, 38000,
			118, 118, 112, 112, 0, 0);

		AddNewRideShow("Feathered Friends in Flight!",           35, RideExitType::Short,       60, 1000, 1000, 12,   10, 389, 2, 70, true, true, false); // temp replaces "UP! A Great Bird Adventure"
		AddNewRideShow("Festival of the Lion King",              30, RideExitType::Short,       60, 1500, 1500,  6, -265, 337, 2, 70, true, true, false);
		AddNewRideShow("Finding Nemo: The Big Blue and Beyond!", 30, RideExitType::Short,       60, 1500, 1500, 11,  290, 205, 2, 70, true, true, false);

		AddNewRideContinuous("Animation Academy",                23, RideExitType::Short,      110,  8, -125, 476, 0,  0, true, true, false);
		AddNewRideContinuous("Gorilla Falls Exploration Trail",  23, RideExitType::Short,     2000,  5, -198, 476, 0,  0, true, true, false);
		AddNewRideContinuous("DINOSAUR",                          4, RideExitType::Shop,      1800,  3,  184,  37, 2, 70, true, true, false);
		AddNewRideContinuous("Expedition Everest",                3, RideExitType::Shop,      1900,  2,  323, 352, 2, 70, true, true, false); // 1800-2060
		AddNewRideContinuous("Flights of Passage",                5, RideExitType::LargeShop, 1300,  1, -227,  30, 1, 70, true, true, false); // 1280-1440; exit then shop
		AddNewRideContinuous("It's tough to be a bug",            8, RideExitType::Short,     1800,  9,  -59, 295, 2, 70, true, true, false);
		AddNewRideContinuous("Kali River Rapids",                 4, RideExitType::Short,     1900,  7,  180, 418, 2, 70, true, true, false); // 1800-2000
		AddNewRideContinuous("Kilimanjaro Safaris",              19, RideExitType::Short,     3000,  5, -211, 444, 2, 70, true, true, false);
		AddNewRideContinuous("Na'vi River Journey",               5, RideExitType::Short,     1200,  4, -160,   0, 1, 70, true, true, false); // 1080-1440
		AddNewRideContinuous("TriceraTop Spin",                   2, RideExitType::Short,      850, 10,  186, 107, 0,  0, true, true, false); // 800-900
		AddNewRideContinuous("Wildlife Express Train",            7, RideExitType::Short,     1500,  8, -123, 476, 0,  0, true, true, false); // 1000-1250 (per station, two trains operating)

		break;

	case ParkTemplate::WDWEpcot:

		SetEntrance("EPCOT", 0, 0, 1, 34000,
			118, 118, 112, 112, 0, 0);

		AddNewRideShow("The American Adventure", 29, RideExitType::Short, 60, 1024, 1024, 9, 0, 925, 0, 0, true, true, false);

		AddNewRideContinuous("Frozen Ever After",                               5, RideExitType::Shop,  1100,  5, -274, 587, 2, 70, true, true, false); // 1100-1200
		AddNewRideContinuous("Gran Fiesta Tour Starring the Three Caballeros",  8, RideExitType::Short, 1800,  6, -245, 435, 0,  0, true, true, false); // 1800-2000
		AddNewRideContinuous("Guardians of the Galaxy: Cosmic Rewind",          4, RideExitType::Short, 2000,  1, -151, 192, 1, 70, true, true, false);
		AddNewRideContinuous("Living With The Land",                           14, RideExitType::Short, 2445,  7,  324, 194, 2, 70, true, true, false);
		AddNewRideContinuous("Mission: SPACE",                                  6, RideExitType::Shop,  1600,  8, -213, 550, 2, 70, true, true, false); // between Green and Orange teams
		AddNewRideContinuous("O Canada!",                                      18, RideExitType::Short,  610, 11,  197, 477, 0,  0, true, true, false);
		AddNewRideContinuous("Remy’s Ratatouille Adventure",                    5, RideExitType::Short, 2000,  4,  320, 820, 0,  0, true, true, false); // 2000-2200
		AddNewRideContinuous("Soarin’ Around the World",                        5, RideExitType::Short, 1827,  2,  273, 332, 1, 70, true, true, false); // 1827 (official), 2000-2100 actual
		AddNewRideContinuous("Spaceship Earth",                                15, RideExitType::Short, 2400,  3,    0,  85, 2, 70, true, true, false);
		AddNewRideContinuous("Test Track",                                      4, RideExitType::Shop,  1600,  1, -177, 337, 1, 70, true, true, false);
		AddNewRideContinuous("The Seas with Nemo & Friends",                    4, RideExitType::Shop,  2000, 10,  145, 124, 0,  0, true, true, false); // 2000-2200
		
		break;

	case ParkTemplate::WDWHollywoodStudios:

		SetEntrance("Hollywood Studios", 0, 0, 1, 31000,
			118, 118, 112, 112, 0, 0);

		AddNewRideShow("Indiana Jones Epic Stunt Spectacular!",            30, RideExitType::Short,   60, 2000, 2000, 10, -190, 0, 2, 70, true, true, false);
		AddNewRideShow("Muppet*Vision 3-D",                                15, RideExitType::Short,   15, 2256, 564, 8, -290, 142, 2, 70, true, true, false); // 564 per show

		AddNewRideContinuous("Alien Swirling Saucers",                      2, RideExitType::Short,  600, 12, -200, 275, 2, 70, true, true, false);
		AddNewRideContinuous("Beauty and the Beast Live on Stage",         30, RideExitType::Shop,   100, 11,  118,  23, 2, 70, true, true, false);
		AddNewRideContinuous("Lightning McQueen's Racing Academy",         10, RideExitType::Short, 1500, 14,  203,  89, 0,  0, true, true, false); // guess
		AddNewRideContinuous("Mickey & Minnie’s Runaway Railway",           5, RideExitType::Short, 1800,  6, -122, 105, 1, 70, true, true, false); // 1800-2000
		AddNewRideContinuous("Millennium Falcon: Smugglers Run",            5, RideExitType::Short, 1700,  7, -405, 412, 1, 70, true, true, false);
		AddNewRideContinuous("Rock ‘n’ Roller Coaster Starring Aerosmith",  2, RideExitType::Shop,  1400,  5,  221,  76, 2, 70, true, true, false); // 1400-1600
		AddNewRideContinuous("Slinky Dog Dash",                             2, RideExitType::Short, 1440,  4,   70, 384, 1, 70, true, true, false);
		AddNewRideContinuous("Star Tours The Adventures Continue",          5, RideExitType::Shop,  1890,  9, -270,  72, 2, 70, true, true, false); // 1800-1950
		AddNewRideContinuous("Star Wars: Rise of the Resistance",          18, RideExitType::Short, 1300,  1, -323, 243, 2, 70, true, true, false); // 1300-1700
		AddNewRideContinuous("The Twilight Zone Tower of Terror",          50, RideExitType::Shop,  1700,  2,  176,   5, 2, 70, true, true, false);
		AddNewRideContinuous("Toy Story Midway Mania!",                     8, RideExitType::Short, 1600,  3, -155, 261, 2, 70, true, true, false); // 1600-1700
		AddNewRideContinuous("Voyage of the Little Mermaid",               15, RideExitType::Short, 1900, 12,  -25, 192, 2, 70, true, true, false);
	            
		break;

	case ParkTemplate::WDWMagicKingdom:

		SetEntrance("Magic Kingdom", 0, 0, 3, 57000,
			118, 118, 112, 112, 0, 0);

		AddNewRideContinuous("it’s a small world",                           11, RideExitType::Short, 2000,  9,  -73, 428, 3, 70, true, true, false); // the best ride in the whole universe
		AddNewRideContinuous("Astro Orbiter",                                 2, RideExitType::Short,  300, 15,  270, 201, 0,  0, true, true, false);
		AddNewRideContinuous("Big Thunder Mountain Railroad",                 4, RideExitType::Short, 1400,  2, -338, 363, 3, 70, true, true, false);
		AddNewRideContinuous("Buzz Lightyear Space Ranger Spin",              4, RideExitType::Shop,  1200,  9,  167, 165, 3, 70, true, true, false);
		AddNewRideContinuous("Dumbo the Flying Elephant",                     2, RideExitType::Short, 1200, 10,  219, 409, 3, 70, true, true, false);
		AddNewRideContinuous("Jungle Cruise",                                10, RideExitType::Short, 1200,  6, -225, 156, 3, 70, true, true, false); // 1200 (official) 1350-1450 actual
		AddNewRideContinuous("Liberty Square Riverboat",                     13, RideExitType::Short,  900, 13, -164, 328, 0,  0, true, true, false);
		AddNewRideContinuous("Mad Tea Party",                                 2, RideExitType::Short,  900, 11,  137, 368, 3, 70, true, true, false); // 900-1000
		AddNewRideContinuous("Peter Pan’s Flight",                            3, RideExitType::Short, 1075,  8,  -77, 407, 3, 70, true, true, false); // 1075-1200
		AddNewRideContinuous("Pirates of the Caribbean",                      9, RideExitType::Shop,  3400,  7, -300, 170, 3, 70, true, true, false);
		AddNewRideContinuous("Prince Charming Regal Carrousel",               2, RideExitType::Short,  700, 12,    0, 377, 0,  0, true, true, false); // 700-1000
		AddNewRideContinuous("Space Mountain",                                3, RideExitType::Shop,  2000,  5,  316, 275, 3, 70, true, true, false);
		AddNewRideContinuous("Splash Mountain",                              11, RideExitType::Shop,  1800,  3, -400, 290, 3, 70, true, true, false);
		AddNewRideContinuous("The Haunted Mansion",                           8, RideExitType::Short, 2400,  1, -186, 408, 3, 70, true, true, false);
		AddNewRideContinuous("The Many Adventures of Winnie the Pooh",        4, RideExitType::Shop,   600, 13,   87, 390, 3, 70, true, true, false); // 600-800
		AddNewRideContinuous("Tomorrowland Transit Authority Peoplemover",   10, RideExitType::Short, 3600,  4,  188, 194, 0,  0, true, true, false);
		AddNewRideContinuous("Under the Sea: Journey of the Little Mermaid",  7, RideExitType::Short, 1800, 17,  142, 472, 3, 70, true, true, false); // 1800-1994
		AddNewRideContinuous("Walt Disney’s Carousel of Progress",           21, RideExitType::Short, 3600, 16,  190, 114, 0,  0, true, true, false); // 240 per show
	                           
		break;

	case ParkTemplate::Arcade:

		SetEntrance("Arcade", 0, 0, 0, 5000,
			        5, 5, 5, 5, 0, 0);

		AddNewRideContinuous("Outrun",            2, RideExitType::Short, 120, 1, -20, 10, 0, 0, true, true, false);
		AddNewRideContinuous("Pac-Man",           2, RideExitType::Short, 120, 1, -10, 10, 0, 0, true, true, false);
		AddNewRideContinuous("Ghosts n Goblins",  2, RideExitType::Short, 120, 1,   0, 10, 0, 0, true, true, false);
		AddNewRideContinuous("Bubble Bobble",     2, RideExitType::Short, 120, 1, -10, 10, 0, 0, true, true, false);
		AddNewRideContinuous("Golden Axe",        2, RideExitType::Short, 120, 1, -20, 10, 0, 0, true, true, false);

		AddNewRideContinuous("Afterburner",       2, RideExitType::Short, 120, 1, -20, 20, 0, 0, true, true, false);
		AddNewRideContinuous("Afterburner II",    2, RideExitType::Short, 120, 1, -10, 20, 0, 0, true, true, false);
		AddNewRideContinuous("Galaxian",          2, RideExitType::Short, 120, 1,   0, 20, 0, 0, true, true, false);
		AddNewRideContinuous("Samurai Shodown 2", 2, RideExitType::Short, 120, 1, -10, 20, 0, 0, true, true, false);
		AddNewRideContinuous("Virtua Cop",        2, RideExitType::Short, 120, 1, -20, 20, 0, 0, true, true, false);

		AddNewRideContinuous("Final Fight",       3, RideExitType::Short, 120, 1, -20, 30, 0, 0, true, true, false);
		AddNewRideContinuous("Op Wolf",           3, RideExitType::Short, 120, 1, -10, 30, 0, 0, true, true, false);
		AddNewRideContinuous("Mortal Kombat",     3, RideExitType::Short, 120, 1,   0, 30, 0, 0, true, true, false);
		AddNewRideContinuous("Growl",             3, RideExitType::Short, 120, 1, -10, 30, 0, 0, true, true, false);
		AddNewRideContinuous("Street Fighter",    3, RideExitType::Short, 120, 1, -20, 30, 0, 0, true, true, false);

		AddNewRideContinuous("Gauntlet",          2, RideExitType::Short, 120, 1, -20, 40, 0, 0, true, true, false);
		AddNewRideContinuous("1942",              2, RideExitType::Short, 120, 1, -10, 40, 0, 0, true, true, false);
		AddNewRideContinuous("Rampage",           2, RideExitType::Short, 120, 1,   0, 40, 0, 0, true, true, false);
		AddNewRideContinuous("Bomb Jack",         2, RideExitType::Short, 120, 1, -10, 40, 0, 0, true, true, false);
		AddNewRideContinuous("Tekken",            2, RideExitType::Short, 120, 1, -20, 40, 0, 0, true, true, false);

		AddNewRideContinuous("Space Invaders",    2, RideExitType::Short, 120, 1, -20, 50, 0, 0, true, true, false);
		AddNewRideContinuous("Powerdrift",        2, RideExitType::Short, 120, 1, -10, 50, 0, 0, true, true, false);
		AddNewRideContinuous("Metal Slug",        2, RideExitType::Short, 120, 1,   0, 50, 0, 0, true, true, false);
		AddNewRideContinuous("R-Type",            2, RideExitType::Short, 120, 1, -10, 50, 0, 0, true, true, false);
		AddNewRideContinuous("Galaga",            2, RideExitType::Short, 120, 1, -20, 50, 0, 0, true, true, false);
		
		break;

	case ParkTemplate::AltonTowers:

		SetEntrance("Alton Towers", 100, 740, 0, 25000, 
					80, 44, 74, 44, 0, 0);

		AddNewRideShow("The Alton Towers Dungeon", 45, RideExitType::Short, 60, 1000, 1000, 10, 0, 0, 0, 0, true, true, false);

		AddNewRideContinuous("The Smiler",                            3, RideExitType::Shop,  1200,  1, -118, 352, 1, 70, true, true,  false);
		AddNewRideContinuous("Nemesis",                               2, RideExitType::Short, 1400,  2,  733, 296, 1, 70, true, true,  false);
		AddNewRideContinuous("Oblivion",                              2, RideExitType::Short, 1900,  3, -170, 268, 1, 70, true, false, false);
		AddNewRideContinuous("Rita",                                  1, RideExitType::Short, 1150,  4,  265,  86, 1, 70, true, false, false);
		AddNewRideContinuous("Th13teen",                              2, RideExitType::Short, 1100,  5,  223,  82, 1, 70, true, true,  false);
		AddNewRideContinuous("Enterprise",                            2, RideExitType::Short, 1400,  6, -196, 361, 1, 70, true, false, false);
		AddNewRideContinuous("Galactica",                             2, RideExitType::Short, 1500,  7,  796, 158, 1, 70, true, false, false);
		AddNewRideContinuous("Wicker Man",                            2, RideExitType::Short,  952,  8,  308, 559, 1, 70, true, true,  false);
		AddNewRideContinuous("Hex: The Legend of the Towers",         6, RideExitType::Short,  500,  9,   77, 156, 1, 70, true, true,  false); // 78 per vault
		AddNewRideContinuous("Spinball Whizzer",                      2, RideExitType::Short,  950, 10,  -30, 523, 1, 70, true, true,  false);

		AddNewRideContinuous("Runaway Train",                         2, RideExitType::Short, 1100, 11,  549, 614, 1, 70, true, true,  false);
		AddNewRideContinuous("Duel: The Haunted House Strikes Back!", 6, RideExitType::Short, 1920, 12,  686, 556, 1, 70, true, true,  false);
		AddNewRideContinuous("Congo River Rapids",                    6, RideExitType::Short, 1800, 13,  442, 565, 1, 70, true, true,  false);
		AddNewRideContinuous("Battle Galleons",                       3, RideExitType::Short,  800, 14,  237, 467, 0,  0, true, true,  false);
		AddNewRideContinuous("The Blade",                             2, RideExitType::Short, 1300, 15,  661, 296, 1, 70, true, true,  false);
		AddNewRideContinuous("Raj's Bouncy Bottom Burp",              2, RideExitType::Short,  200, 16,    0, 127, 0,  0, true, true,  false);
		AddNewRideContinuous("Heave Ho!",                             2, RideExitType::Short,  600, 17,  216, 601, 0,  0, true, true,  false);
		AddNewRideContinuous("The Royal Carousel",                    2, RideExitType::Short,  450, 18,   -7,  51, 0,  0, true, true,  false);
		AddNewRideContinuous("Marauder's Mayhem",                     2, RideExitType::Short, 1200, 19,  244, 594, 0,  0, true, true,  false);
		AddNewRideContinuous("Spinjam",                               2, RideExitType::Short,  950, 20, -195, 317, 0,  0, true, true,  false);

		AddNewRideContinuous("Gangsta Granny: The Ride",              2, RideExitType::Short, 1100, 21,   23,  87, 1, 70, true, true,  false);
		AddNewRideContinuous("Flavio's Fabulous Fandango",            2, RideExitType::Short,  950, 22,   57,  75, 0,  0, true, true,  false); //guess
		AddNewRideContinuous("Cuckoo Cars Driving School",            4, RideExitType::Short,  450, 23,   41, 125, 0,  0, true, true,  false);
		AddNewRideContinuous("Octonauts Rollercoaster Adventure",     2, RideExitType::Short,  580, 24,   40, 567, 0,  0, true, true,  false);
		AddNewRideContinuous("Funk'n'Fly",                            2, RideExitType::Short,  480, 25,  700, 245, 0,  0, true, true,  false);
		AddNewRideContinuous("Skyride",                               5, RideExitType::Short, 2800, 26,  158, 618, 0,  0, true, true,  false);

		break;
	}

	UpdateThroughput();
}


int ParkController::GetLargestValue(int parameter)
{
	int result = 0;

	for (int r = 0; r < Rides.size(); r++)
	{
		switch (parameter)
		{
		case kGetMaxQueueLength:
			if (Rides[r].DailyStatistics.maxQueueLength > result)
			{
				result = Rides[r].DailyStatistics.maxQueueLength;
			}
		}
	}

	return result;
}


void ParkController::OutputStatus(const std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


// =================================================================================================================
// == Purchases ====================================================================================================
// =================================================================================================================


int ParkController::BuyTicket(AgeGroup age, bool advanced)
{
	int cost = 0;

	switch (age)
	{
	case AgeGroup::Baby:
		if (advanced)
		{
			cost = entrance.Prices.BabiesAdvance;

			entrance.Tickets.BabiesAdvance++;
		}
		else
		{
			cost = entrance.Prices.Babies;

			entrance.Tickets.Babies++;
		}
		break;

	case AgeGroup::Child:
		if (advanced)
		{
			cost = entrance.Prices.ChildrenAdvance;

			entrance.Tickets.ChildrenAdvance++;
		}
		else
		{
			cost = entrance.Prices.Children;

			entrance.Tickets.Children++;
		}
		break;

	case AgeGroup::Adult:
		if (advanced)
		{
			cost = entrance.Prices.AdultAdvance;

			entrance.Tickets.AdultAdvance++;
		}
		else
		{
			cost = entrance.Prices.Adult;

			entrance.Tickets.Adult++;
		}
		break;
	}

	return cost;
}


// =================================================================================================================
// == File I/O =====================================================================================================
// =================================================================================================================


bool ParkController::BuildFromTemplate(const std::string file_name)
{
	Ini* config = new Ini(file_name);

	if (config->Loaded)
	{
		std::string name = config->ReadString("main", "name", "The Park with No Name");
		int ex = config->ReadInteger("main", "entrancex", 0);
		int ey = config->ReadInteger("main", "entrancey", 0);
		int fptype = config->ReadInteger("main", "fastpasttype", 0);
		int ridecount = config->ReadInteger("main", "ridecount", 0);
		int average_visitors = config->ReadInteger("main", "averagevisitors", 0);

		int adult = config->ReadInteger("main", "adult", 0);
		int adult_advanced = config->ReadInteger("main", "adultadvanced", 0);
		int child = config->ReadInteger("main", "child", 0);
		int child_advanced = config->ReadInteger("main", "childadvanced", 0);
		int baby = config->ReadInteger("main", "baby", 0);
		int baby_advanced = config->ReadInteger("main", "babyadvanced", 0);

		SetEntrance(name, ex, ey, fptype, average_visitors, adult, adult_advanced, child, child_advanced, baby, baby_advanced);

		if (ridecount != 0)
		{
			for (int r = 1; r <= ridecount; r++)
			{
				std::string name = config->ReadString("ride" + std::to_string(r), "name", "unknown");
				std::string type = config->ReadString("ride" + std::to_string(r), "type", "c");

				if (type == "s")
				{
					int l = config->ReadInteger("ride" + std::to_string(r), "length", 0);
					RideExitType e = QWaitTypes::IntToRideExitType(config->ReadInteger("ride" + std::to_string(r), "rideexittype", 0));
					int s = config->ReadInteger("ride" + std::to_string(r), "starttimemins", 0);
					int h = config->ReadInteger("ride" + std::to_string(r), "hourlythroughput", 0);
					int c = config->ReadInteger("ride" + std::to_string(r), "showcapacity", 0);
					int p = config->ReadInteger("ride" + std::to_string(r), "popularity", 0);
					int x = config->ReadInteger("ride" + std::to_string(r), "x", 0);
					int y = config->ReadInteger("ride" + std::to_string(r), "y", 0);
					int i = config->ReadInteger("ride" + std::to_string(r), "isfastpass", 0);
					int f = config->ReadInteger("ride" + std::to_string(r), "fpreservepercent", 0);

					int u = config->ReadBoolean("ride" + std::to_string(r), "adultvalid", true);
					int v = config->ReadBoolean("ride" + std::to_string(r), "childvalid", true);
					int w = config->ReadBoolean("ride" + std::to_string(r), "babyvalid", false);

					if (l == 0 || h == 0 || s == 0 || f == 0)
					{
						std::cout << name << " : length, hourlythroughput, showcapacity, and fpreservepercent cannot be zero." << std::endl;
					}
					else
					{
						AddNewRideShow(name, l, e, s, h, c, p, x, y, i, f, u, v, w);
					}
				}
				else if (type == "c")
				{
					int l = config->ReadInteger("ride" + std::to_string(r), "length", 0);
					int h = config->ReadInteger("ride" + std::to_string(r), "hourlythroughput", 0);
					RideExitType e = QWaitTypes::IntToRideExitType(config->ReadInteger("ride" + std::to_string(r), "rideexittype", 0));
					int p = config->ReadInteger("ride" + std::to_string(r), "popularity", 0);
					int x = config->ReadInteger("ride" + std::to_string(r), "x", 0);
					int y = config->ReadInteger("ride" + std::to_string(r), "y", 0);
					int i = config->ReadInteger("ride" + std::to_string(r), "isfastpass", 0);
					int f = config->ReadInteger("ride" + std::to_string(r), "fpreservepercent", 0);

					int u = config->ReadBoolean("ride" + std::to_string(r), "adultvalid", true);
					int v = config->ReadBoolean("ride" + std::to_string(r), "childvalid", true);
					int w = config->ReadBoolean("ride" + std::to_string(r), "babyvalid", false);

					if (l == 0 || h == 0 || f == 0)
					{
						std::cout << name << " : length, hourlythroughput, and fpreservepercent cannot be zero." << std::endl;
					}
					else
					{
						AddNewRideContinuous(name, l, e, h, p, x, y, i, f, u, v, w);
					}
				}
				else
				{
					std::cerr << name << " : invalid type (should be c or s) \"" << type << "\"" << std::endl;
				}
			}

			UpdateThroughput();
		}
		else
		{
			std::cerr << name << " : ridecount cannot be zero!" << std::endl;
		}
	}
	else
	{
		delete config;

		return false;
	}

	delete config;

	return true;
}


void ParkController::SaveAsTemplate(const std::string file_name)
{
	std::ofstream file(file_name);

	if (file)
	{
		OutputStatus("Saving as Template file...");

		file << "// QWait (" << Constants::QWVersion << ") Ride Template File" << "\n";
		file << "//   created " << Utility::DateTime(kDisplayModeConsole) << "\n";
		file << "// https://github.com/MaximumOctopus/QWait" << "\n";
		file << "\n";
		file << "[main]" << "\n";
		file << "name=" << entrance.ParkName << "\n";
		file << "entrancex=" << entrance.position.x << "\n";
		file << "entrancey=" << entrance.position.y << "\n";
		file << "fastpasttype=" << entrance.fastPassType << "\n";
		file << "ridecount=" << Rides.size() << "\n";
		file << "averagevisitors=" << entrance.averageVisitors << "\n";

		file << "adult=" << entrance.Prices.Adult << "\n";
		file << "adultadvanced=" << entrance.Prices.AdultAdvance << "\n";
		file << "child=" << entrance.Prices.Children << "\n";
		file << "childadvanced=" << entrance.Prices.ChildrenAdvance << "\n";
		file << "baby=" << entrance.Prices.Babies << "\n";
		file << "babyadvanced=" << entrance.Prices.BabiesAdvance << "\n";

		file << "\n";

		for (int r = 0; r < Rides.size(); r++)
		{
			Ride ride = Rides[r];

			file << "[ride" << r + 1 << "]" << "\n";
			file << "name=" << ride.RideOperation.name << "\n";

			if (ride.RideOperation.rideType == RideType::Continuous)
			{
				file << "type=c" << "\n";
				file << "length=" << ride.RideOperation.rideLength << "\n";
				file << "rideexittype=" << QWaitTypes::RideExitTypeToInt(ride.RideOperation.rideExitType) << "\n";
				file << "hourlythroughput=" << ride.RideThroughput.totalPerHour << "\n";
			}
			else if (ride.RideOperation.rideType == RideType::Show)
			{
				file << "type=s" << "\n";
				file << "length=" << ride.RideOperation.rideLength << "\n";
				file << "rideexittype=" << QWaitTypes::RideExitTypeToInt(ride.RideOperation.rideExitType) << "\n";
				file << "starttimemins=" << ride.RideOperation.ShowStartTime << "\n";
				file << "hourlythroughput=" << ride.RideThroughput.totalPerHour << "\n";
				file << "showcapacity=" << ride.RideThroughput.showCapacity << "\n";
			}

			file << "popularity=" << ride.RideOperation.Popularity << "\n";
			file << "x=" << ride.RideOperation.position.x << "\n";
			file << "y=" << ride.RideOperation.position.y << "\n";
			file << "isfastpass=" << ride.FastPass.mode << "\n";
			file << "fpreservepercent=" << ride.FastPass.percentage << "\n";

			file << "adultvalid=" << ride.RideOperation.AdultValid << "\n";
			file << "childvalid=" << ride.RideOperation.ChildValid << "\n";
			file << "babyvalid=" << ride.RideOperation.BabyValid << "\n";

			file << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Error saving Template file \"" + file_name + "\"." << std::endl;
	}
}


bool ParkController::SaveDistanceCache(const std::string file_name)
{
	std::ofstream file(file_name);

	if (file)
	{
		file << "// ==================================================\n";
		file << "// walk distance cache\n";
		file << "// ==================================================\n";

		for (int y = 0; y < Rides.size() + 1; y++)
		{
			std::string row = "";

			if (y < Rides.size())
			{
				row = Rides[y].RideOperation.name;
			}
			else
			{
				row = "Park Entrance";
			}

			for (int x = 0; x < Rides.size() + 1; x++)
			{
				row += "," + std::to_string(WalkDistanceCache[x][y]);
			}

			file << row << "\n";

		}

		file << "// ==================================================\n";
		file << "// walk time cache\n";
		file << "// ==================================================\n";

		for (int y = 0; y < Rides.size() + 1; y++)
		{
			std::string row = "";

			if (y < Rides.size())
			{
				row = Rides[y].RideOperation.name;
			}
			else
			{
				row = "Park Entrance";
			}

			for (int x = 0; x < Rides.size() + 1; x++)
			{
				row += "," + std::to_string(WalkTimeCache[x][y]);
			}

			file << row << "\n";

		}

		file.close();

		return true;
	}
	else
	{
		std::cerr << "Error saving Distance/Time Cache file \"" + file_name + "\"." << std::endl;
	}

	return false;
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


int ParkController::GetLargestTicketCategory()
{
	int result = GParkController->entrance.Tickets.Adult;

	if (GParkController->entrance.Tickets.Children > result)
	{
		result = GParkController->entrance.Tickets.Children;
	}
	else if (GParkController->entrance.Tickets.Babies > result)
	{
		result = GParkController->entrance.Tickets.Babies;
	}
	else if (GParkController->entrance.Tickets.AdultAdvance > result)
	{
		result = GParkController->entrance.Tickets.AdultAdvance;
	}
	else if (GParkController->entrance.Tickets.ChildrenAdvance > result)
	{
		result = GParkController->entrance.Tickets.ChildrenAdvance;
	}
	else if (GParkController->entrance.Tickets.BabiesAdvance > result)
	{
		result = GParkController->entrance.Tickets.BabiesAdvance;
	}

	return result;
}


void ParkController::ShowStatistics(int park_open_hours)
{
	if (ShowOutput)
	{
		std::cout << "\n";

		int TotalRidersToday = 0;

		for (int r = 0; r < Rides.size(); r++)
		{
			int pc = static_cast<int>((((float)Rides[r].DailyStatistics.totalRiders / ((float)park_open_hours + 1.0f)) / (float)Rides[r].RideThroughput.totalPerHour) * (float)100);
			int ph = static_cast<int>(Rides[r].DailyStatistics.totalRiders / (park_open_hours + 1));

			std::cout << Utility::PadRight(ph, 6) << " riders per hour vs " << Utility::PadRight(Rides[r].RideThroughput.totalPerHour, 5) << " (" << Utility::PadRight(pc, 3) << "%), longest q " << Utility::PadRight(Rides[r].DailyStatistics.maxQueueLength, 6) << " " << Rides[r].RideOperation.name << "\n";

			TotalRidersToday += Rides[r].DailyStatistics.totalRiders;
		}

		std::cout << "\n" << "Total rides today: " << TotalRidersToday << " (theortical: " << TheoreticalHourThroughputTotal * (park_open_hours + 1) << ")" << std::endl;
	}
}