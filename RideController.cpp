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
// 
//   https://www.disneytouristblog.com/paid-lightning-lanes-genie-replace-free-fastpass-disney-world/
//
//	 https://www.wdwinfo.com/wdwinfo/fastpass.htm
//   https://forums.wdwmagic.com/threads/percentage-of-fps-allocated-for-rides-wdw.955357/


#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include "Configuration.h"
#include "Constants.h"
#include "Ini.h"
#include "Ride.h"
#include "RideController.h"
#include "Utility.h"


extern Configuration* GConfiguration;


RideController* GRideController;


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


RideController::RideController(bool show_config, std::string ride_template_file)
{
	ParkName = "None.";

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


void RideController::SetEntrance(std::string name, int x, int y, int fast_pass_type, int average_visitors)
{
	ParkName = name;

	entrance.position.x = x;
	entrance.position.y = y;
	entrance.throughPutMinute = 100;

	entrance.fastPassType = fast_pass_type;

	entrance.averageVisitors = average_visitors;
}


void RideController::AddNewRideContinuous(std::string name, int ride_length, int hourly_throughput, int popularity, int x, int y, int is_fast_pass, int fp_reserve_percent)
{
	if (FastPassMode == 0)
	{ 
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { GConfiguration->ParkOpenTime, 0 };
	QWaitTypes::Time close = { GConfiguration->ParkCloseTime, 0 };

	int ophours = GConfiguration->ParkOpenHours;

	Ride r(kRideTypeContinuous, name, ride_length, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void RideController::AddNewRideShow(std::string name, int ride_length, int StartTimeMinutes, int hourly_throughput, int show_capacity, int popularity, int x, int y, int IsFastPass, int fp_reserve_percent)
{
	if (FastPassMode == 0)
	{
		IsFastPass = 0;
	}

	QWaitTypes::Time open = { GConfiguration->ParkOpenTime, 0 };
	QWaitTypes::Time close = { GConfiguration->ParkCloseTime, 0 };

	int ophours = GConfiguration->ParkOpenHours;

	Ride r(kRideTypeShow, name, ride_length, hourly_throughput, popularity, x, y, IsFastPass, fp_reserve_percent, open, close, ophours);

	r.RideOperation.ShowStartTime = StartTimeMinutes;

	r.ConfigureShowThroughput(show_capacity, fp_reserve_percent);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void RideController::AddNewRideContinuousWithHours(std::string name, int ride_length, int hourly_throughput, int popularity, int open_hour, int open_minute, int close_hour, int close_minute, int x, int y, int is_fast_pass, int fp_reserve_percent)
{
	if (FastPassMode == 0)
	{
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { open_hour, open_minute };
	QWaitTypes::Time close = { close_hour, close_minute };

	int ophours = Utility::Duration(open, close);

	Ride r(kRideTypeContinuous, name, ride_length, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void RideController::AddNewRideShowWithHours(std::string name, int ride_length, int start_time_minutes, int hourly_throughput, int show_capacity, int popularity, int open_hour, int open_minute, int close_hour, int close_minute, int x, int y, int is_fast_pass, int fp_reserve_percent)
{
	if (FastPassMode == 0)
	{
		is_fast_pass = 0;
	}

	QWaitTypes::Time open = { open_hour, open_minute };
	QWaitTypes::Time close = { close_hour, close_minute };

	int ophours = Utility::Duration(open, close);

	Ride r(kRideTypeShow, name, ride_length, hourly_throughput, popularity, x, y, is_fast_pass, fp_reserve_percent, open, close, ophours);

	r.RideOperation.ShowStartTime = start_time_minutes;

	r.ConfigureShowThroughput(show_capacity, fp_reserve_percent);

	Rides.push_back(r);

	TheoreticalHourThroughputTotal += hourly_throughput;
}


void RideController::BuildRides(int park_template, std::string filename)
{
	if (filename != "")
	{
		BuildFromTemplate(filename);
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


void RideController::AddToLog(std::string log_entry)
{
	Log.push_back(log_entry);
}


int RideController::GetDistanceBetweenInMinutes(int ride1, int ride2)
{
	int Location1 = ride1;
	int Location2 = ride2;
	
	if (ride1 == kNoSelectedRide)
	{
		Location1 = Rides.size();
	}

	if (ride2 == kNoSelectedRide)
	{
		Location2 = Rides.size();
	}

	return WalkTimeCache[Location1][Location2];
}


int RideController::GetDistanceBetweenInMetres(int ride1, int ride2)
{
	int Location1 = ride1;
	int Location2 = ride2;

	if (ride1 == kNoSelectedRide)
	{
		Location1 = Rides.size();
	}

	if (ride2 == kNoSelectedRide)
	{
		Location2 = Rides.size();
	}

	return WalkDistanceCache[Location1][Location2];
}


void RideController::BuildDistanceCache()
{
	static const int WalkSpeedMetresPerMinute = 70;

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
				WalkTimeCache[r1][r2]     = static_cast<int>(sqrt((double)dx * dx + dy * dy) / WalkSpeedMetresPerMinute) + 3;
				WalkDistanceCache[r1][r2] = static_cast<int>(sqrt((double)dx * dx + dy * dy));
			}
		}
	}
}


void RideController::BuildClosestCache()
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


void RideController::BuildSelectionChoiceCache()
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
			std::normal_distribution <double> d(static_cast<int>(GRideController->Rides.size() * Constants::VisitorTypesRidePreference[t]), Constants::VisitorTypesRidePreferenceStdDev[t]);

			SelectedRide = d(gen);

			if (SelectedRide < 0)
			{
				SelectedRide = abs(SelectedRide);
			}

			if (SelectedRide >= 0 && SelectedRide <= GRideController->Rides.size() - 1)
			{
				SelectionChoiceCache[t][count] = SelectedRide;

				count++;
			}
		}
	}
}


// add this ride to the correct fastpass list type 1, type 2, or type 3
// already ordered by popularity
void RideController::BuildFastPassList()
{
	for (int r = 0; r < Rides.size(); r++)
	{
		if (Rides[r].FastPass.mode != 0)
		{
			FastPassType[Rides[r].FastPass.mode - 1].push_back(r);
		}
	}
}


size_t RideController::EntranceQueueSize()
{
	return EntranceQueue.size();
}


void RideController::AddToEntranceQueue(int visitorID)
{
	EntranceQueue.push_back(visitorID);
}


int RideController::RemoveFromEntranceQueue()
{
	if (EntranceQueue.size() != 0)
	{
		int VisitorID = EntranceQueue[0];

		EntranceQueue.erase(EntranceQueue.begin());

		return VisitorID;
	}

	return kNotValidVisitor;
}


int RideController::GetMinuteCount()
{
	return Minutes - 1;
}


void RideController::UpdateThroughput()
{
	TheoreticalHourThroughputTotalDay = TheoreticalHourThroughputTotal * GConfiguration->ParkOpenHours;
}


void RideController::ShowConfig()
{
	if (ShowOutput)
	{
		std::cout << "Park Name                    : " << ParkName << "\n";
		std::cout << "Number of rides in park      : " << Rides.size() << "\n";
		std::cout << "Theoretical hourly throughput: " << TheoreticalHourThroughputTotal << "\n";
		std::cout << "Theoretical daily throughput : " << TheoreticalHourThroughputTotalDay << "\n";
		std::cout << std::endl;
	}
}


bool RideController::BuildFromTemplate(std::string filename)
{
	Ini* config = new Ini(filename);

	if (config->Loaded)
	{
		std::string name = config->ReadString("main", "name", "unknown");
		int ex = config->ReadInteger("main", "entrancex", 0);
		int ey = config->ReadInteger("main", "entrancey", 0);
		int fptype = config->ReadInteger("main", "fastpasttype", 0);
		int ridecount = config->ReadInteger("main", "ridecount", 0);
		int average_visitors = config->ReadInteger("main", "averagevisitors", 0);

		SetEntrance(name, ex, ey, fptype, average_visitors);

		if (ridecount != 0)
		{
			for (int r = 1; r <= ridecount; r++)
			{
				std::string name = config->ReadString("ride" + std::to_string(r), "name", "unknown");
				std::string type = config->ReadString("ride" + std::to_string(r), "type", "c");

				if (type == "s")
				{
					int l = config->ReadInteger("ride" + std::to_string(r), "length", 0);
					int s = config->ReadInteger("ride" + std::to_string(r), "starttimemins", 0);
					int h = config->ReadInteger("ride" + std::to_string(r), "hourlythroughput", 0);
					int c = config->ReadInteger("ride" + std::to_string(r), "showcapacity", 0);
					int p = config->ReadInteger("ride" + std::to_string(r), "popularity", 0);
					int x = config->ReadInteger("ride" + std::to_string(r), "x", 0);
					int y = config->ReadInteger("ride" + std::to_string(r), "y", 0);
					int i = config->ReadInteger("ride" + std::to_string(r), "isfastpass", 0);
					int f = config->ReadInteger("ride" + std::to_string(r), "fpreservepercent", 0);

					if (l == 0 || h == 0 || s == 0 || f == 0)
					{
						std::cout << name << " : length, hourlythroughput, showcapacity, and fpreservepercent cannot be zero." << std::endl;
					}
					else
					{
						AddNewRideShow(name, l, s, h, c, p, x, y, i, f);
					}
				}
				else if (type == "c")
				{
					int l = config->ReadInteger("ride" + std::to_string(r), "length", 0);
					int h = config->ReadInteger("ride" + std::to_string(r), "hourlythroughput", 0);
					int p = config->ReadInteger("ride" + std::to_string(r), "popularity", 0);
					int x = config->ReadInteger("ride" + std::to_string(r), "x", 0);
					int y = config->ReadInteger("ride" + std::to_string(r), "y", 0);
					int i = config->ReadInteger("ride" + std::to_string(r), "isfastpass", 0);
					int f = config->ReadInteger("ride" + std::to_string(r), "fpreservepercent", 0);

					if (l == 0 || h == 0 || f == 0)
					{
						std::cout << name << " : length, hourlythroughput, and fpreservepercent cannot be zero." << std::endl;
					}
					else
					{
						AddNewRideContinuous(name, l, h, p, x, y, i, f);
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


void RideController::SaveAsTemplate(std::string filename)
{
	std::ofstream file(filename);

	if (file)
	{
		OutputStatus("Saving as Template file...");

		file << "// QWait (" << Constants::QWVersion << ") Ride Template File" << "\n";
		file << "//   created " << Utility::DateTime(0) << "\n";
		file << "\n";
		file << "[main]" << "\n";
		file << "name=" << ParkName << "\n";
		file << "entrancex=" << entrance.position.x << "\n";
		file << "entrancey=" << entrance.position.y << "\n";
		file << "fastpasttype=" << entrance.fastPassType << "\n";
		file << "ridecount=" << Rides.size() << "\n";
		file << "\n";

		for (int r = 0; r < Rides.size(); r++)
		{
			Ride ride = Rides[r];

			file << "[ride" << r + 1 << "]" << "\n";
			file << "name=" << ride.RideOperation.name << "\n";

			if (ride.RideOperation.rideType == kRideTypeContinuous)
			{
				file << "type=c" << "\n";
				file << "length=" << ride.RideOperation.rideLength << "\n";
				file << "hourlythroughput=" << ride.RideThroughput.totalPerHour << "\n";
			}
			else if (ride.RideOperation.rideType == kRideTypeShow)
			{
				file << "type=s" << "\n";
				file << "length=" << ride.RideOperation.rideLength << "\n";
				file << "starttimemins=" << ride.RideOperation.ShowStartTime << "\n";
				file << "hourlythroughput=" << ride.RideThroughput.totalPerHour << "\n";
				file << "showcapacity=" << ride.RideThroughput.showCapacity << "\n";
			}

			file << "popularity=" << ride.RideOperation.Popularity << "\n";
			file << "x=" << ride.RideOperation.position.x << "\n";
			file << "y=" << ride.RideOperation.position.y << "\n";
			file << "isfastpass=" << ride.FastPass.mode << "\n";
			file << "fpreservepercent=" << ride.FastPass.percentage << "\n";

			file << "\n";
		}

		file.close();
	}
	else
	{
		std::cerr << "Error saving Template file \"" + filename + "\"." << std::endl;
	}
}


void RideController::BuildFromTemplate(int park_template)
{
	switch (park_template)
	{
	case Constants::TemplateGenericTest:

		SetEntrance("Super Cat Park", 0, 0, 0, 10000);

		AddNewRideShow("Cats throughout the ages", 30, 60, 2000, 2000, 1, -200, 50, 0, 0);
		AddNewRideShow("Idea of a certain cat", 10, 15, 1000, 250, 12, 3, 300, 0, 0);

		AddNewRideContinuous("Katzen-Kogl", 4, 1800, 4, -200, 150, 0, 0);
		AddNewRideContinuous("Le chat blanc", 4, 1800, 2, 200, 150, 0, 0);
		AddNewRideContinuous("Raminou", 4, 1800, 10, -200, 250, 0, 0);
		AddNewRideContinuous("White cat playing with a string", 4, 1800, 7, 200, 250, 0, 0);
		AddNewRideContinuous("Rutherford Freeman Maxwell", 4, 1800, 8, 200, 50, 0, 0);
		AddNewRideContinuous("Siamese adventure", 4, 1800, 9, 0, 100, 0, 0);
		AddNewRideContinuous("Fluffy cat surprise", 4, 1800, 5, 0, 200, 0, 0);
		AddNewRideContinuous("Meow meow meow!", 4, 1800, 6, 0, 50, 0, 0);

		break;

	case Constants::TemplateWDWAnimalKingdom:

		SetEntrance("Animal Kingdom", 0, 0, 1, 38000);

		AddNewRideShow("Feathered Friends in Flight!", 35, 60, 1000, 1000, 12, 10, 389, 2, 70); // temp replaces "UP! A Great Bird Adventure"
		AddNewRideShow("Festival of the Lion King", 30, 60, 1500, 1500, 6, -265, 337, 2, 70);
		AddNewRideShow("Finding Nemo: The Big Blue and Beyond!", 30, 60, 1500, 1500, 11, 290, 205, 2, 70);

		AddNewRideContinuous("Animation Academy", 23, 110, 8, -125, 476, 0, 0);
		AddNewRideContinuous("Gorilla Falls Exploration Trail", 23, 2000, 5, -198, 476, 0, 0);
		AddNewRideContinuous("DINOSAUR", 4, 1800, 3, 184, 37, 2, 70);
		AddNewRideContinuous("Expedition Everest", 3, 1900, 2, 323, 352, 2, 70); // 1800-2060
		AddNewRideContinuous("Flight of Passage", 5, 1300, 1, -227, 30, 1, 70); // 1280-1440
		AddNewRideContinuous("It's tough to be a bug", 8, 1800, 9, -59, 295, 2, 70);
		AddNewRideContinuous("Kali River Rapids", 4, 1900, 7, 180, 418, 2, 70); // 1800-2000
		AddNewRideContinuous("Kilimanjaro Safaris", 19, 3000, 5, -211, 444, 2, 70);
		AddNewRideContinuous("Na'vi River Journey", 5, 1200, 4, -160, 0, 1, 70); // 1080-1440
		AddNewRideContinuous("TriceraTop Spin", 2, 850, 10, 186, 107, 0, 0); // 800-900
		AddNewRideContinuous("Wildlife Express Train", 7, 1500, 8, -123, 476, 0, 0); // 1000-1250 (per station, two trains operating)

		break;

	case Constants::TemplateWDWEpcot:

		SetEntrance("EPCOT", 0, 0, 1, 34000);

		AddNewRideShow("The American Adventure", 29, 60, 1024, 1024, 9, 0, 925, 0, 0);

		AddNewRideContinuous("Frozen Ever After", 5, 1100, 5, -274, 587, 2, 70); // 1100-1200
		AddNewRideContinuous("Gran Fiesta Tour Starring the Three Caballeros", 8, 1800, 6, -245, 435, 0, 0); // 1800-2000
		AddNewRideContinuous("Living With The Land", 14, 2445, 7, 324, 194, 2, 70);
		AddNewRideContinuous("Mission: SPACE", 6, 1600, 8, -213, 550, 2, 70); // between Green and Orange teams
		AddNewRideContinuous("O Canada!", 18, 610, 11, 197, 477, 0, 0);
		AddNewRideContinuous("Remy’s Ratatouille Adventure", 5, 2000, 4, 320, 820, 0, 0); // 2000-2200
		AddNewRideContinuous("Soarin’ Around the World", 5, 1827, 2, 273, 332, 1, 70); // 1827 (official), 2000-2100 actual
		AddNewRideContinuous("Spaceship Earth", 15, 2400, 3, 0, 85, 2, 70);
		AddNewRideContinuous("Test Track", 4, 1600, 1, -177, 337, 1, 70);
		AddNewRideContinuous("The Seas with Nemo & Friends", 4, 2000, 10, 145, 124, 0, 0); // 2000-2200
		
		break;

	case Constants::TemplateWDWHollywoodStudios:

		SetEntrance("Hollywood Studios", 0, 0, 1, 31000);

		AddNewRideShow("Indiana Jones Epic Stunt Spectacular!",            30, 60, 2000, 2000, 10, -190, 0, 2, 70); 
		AddNewRideShow("Muppet*Vision 3-D",                                15, 15, 2256, 564, 8, -290, 142, 2, 70); // 564 per show

		AddNewRideContinuous("Alien Swirling Saucers",                      2,  600, 12, -200, 275, 2, 70);
		AddNewRideContinuous("Beauty and the Beast Live on Stage",         30,  100, 11,  118,  23, 2, 70);
		AddNewRideContinuous("Lightning McQueen's Racing Academy",         10, 1500, 14,  203,  89, 0,  0); // guess
		AddNewRideContinuous("Mickey & Minnie’s Runaway Railway",           5, 1800,  6, -122, 105, 1, 70); // 1800-2000
		AddNewRideContinuous("Millennium Falcon: Smugglers Run",            5, 1700,  7, -405, 412, 1, 70);
		AddNewRideContinuous("Rock ‘n’ Roller Coaster Starring Aerosmith",  2, 1400,  5,  221,  76, 2, 70); // 1400-1600
		AddNewRideContinuous("Slinky Dog Dash",                             2, 1440,  4,   70, 384, 1, 70);
		AddNewRideContinuous("Star Tours The Adventures Continue",          5, 1890,  9, -270,  72, 2, 70); // 1800-1950
		AddNewRideContinuous("Star Wars: Rise of the Resistance",          18, 1300,  1, -323, 243, 2, 70); // 1300-1700
		AddNewRideContinuous("The Twilight Zone Tower of Terror",          50, 1700,  2,  176,   5, 2, 70);
		AddNewRideContinuous("Toy Story Midway Mania!",                     8, 1600,  3, -155, 261, 2, 70); // 1600-1700
		AddNewRideContinuous("Voyage of the Little Mermaid",               15, 1900, 12,  -25, 192, 2, 70);

		break;

	case Constants::TemplateWDWMagicKingdom:

		SetEntrance("Magic Kingdom", 0, 0, 3, 57000);

		AddNewRideContinuous("it’s a small world",                           11, 2000,  9,  -73, 428, 3, 70); // the best ride in the whole universe
		AddNewRideContinuous("Astro Orbiter",                                 2,  300, 15,  270, 201, 0,  0);
		AddNewRideContinuous("Big Thunder Mountain Railroad",                 4, 1400,  2, -338, 363, 3, 70);
		AddNewRideContinuous("Dumbo the Flying Elephant",                     2, 1200, 10,  219, 409, 3, 70);
		AddNewRideContinuous("Jungle Cruise",                                10, 1200,  6, -225, 156, 3, 70); // 1200 (official) 1350-1450 actual
		AddNewRideContinuous("Liberty Square Riverboat",                     13,  900, 13, -164, 328, 0,  0);
		AddNewRideContinuous("Mad Tea Party",                                 2,  900, 11,  137, 368, 3, 70); // 900-1000
		AddNewRideContinuous("Peter Pan’s Flight",                            3, 1075,  8,  -77, 407, 3, 70); // 1075-1200
		AddNewRideContinuous("Pirates of the Caribbean",                      9, 3400,  7, -300, 170, 3, 70);
		AddNewRideContinuous("Prince Charming Regal Carrousel",               2,  700, 12,    0, 377, 0,  0); // 700-1000
		AddNewRideContinuous("Space Mountain",                                3, 2000,  5,  316, 275, 3, 70);
		AddNewRideContinuous("Splash Mountain",                              11, 1800,  3, -400, 290, 3, 70);
		AddNewRideContinuous("The Haunted Mansion",                           8, 2400,  1, -186, 408, 3, 70);
		AddNewRideContinuous("The Many Adventures of Winnie the Pooh",        4,  600, 13,   87, 390, 3, 70); // 600-800
		AddNewRideContinuous("Tomorrowland Transit Authority Peoplemover",   10, 3600,  4,  188, 194, 0,  0);
		AddNewRideContinuous("Under the Sea: Journey of the Little Mermaid",  7, 1800, 17,  142, 472, 3, 70); // 1800-1994
		AddNewRideContinuous("Walt Disney’s Carousel of Progress",           21, 3600, 16,  190, 114, 0,  0); // 240 per show

		break;

	case Constants::TemplateArcade:

		SetEntrance("Arcade", 0, 0, 0, 5000);

		AddNewRideContinuous("Outrun", 2, 120, 1, -20, 10, 0, 0);
		AddNewRideContinuous("Pac-Man", 2, 120, 1, -10, 10, 0, 0);
		AddNewRideContinuous("Ghosts n Goblins", 2, 120, 1, 0, 10, 0, 0);
		AddNewRideContinuous("Bubble Bobble", 2, 120, 1, -10, 10, 0, 0);
		AddNewRideContinuous("Golden Axe", 2, 120, 1, -20, 10, 0, 0);

		AddNewRideContinuous("Afterburner", 2, 120, 1, -20, 20, 0, 0);
		AddNewRideContinuous("Afterburner II", 2, 120, 1, -10, 20, 0, 0);
		AddNewRideContinuous("Galaxian", 2, 120, 1, 0, 20, 0, 0);
		AddNewRideContinuous("Samurai Shodown 2", 2, 120, 1, -10, 20, 0, 0);
		AddNewRideContinuous("Virtua Cop", 2, 120, 1, -20, 20, 0, 0);

		AddNewRideContinuous("Final Fight", 3, 120, 1, -20, 30, 0, 0);
		AddNewRideContinuous("Op Wolf", 3, 120, 1, -10, 30, 0, 0);
		AddNewRideContinuous("Mortal Kombat", 3, 120, 1,   0, 30, 0, 0);
		AddNewRideContinuous("Growl", 3, 120, 1, -10, 30, 0, 0);
		AddNewRideContinuous("Street Fighter", 3, 120, 1, -20, 30, 0, 0);

		AddNewRideContinuous("Gauntlet", 2, 120, 1, -20, 40, 0, 0);
		AddNewRideContinuous("1942", 2, 120, 1, -10, 40, 0, 0);
		AddNewRideContinuous("Rampage", 2, 120, 1,   0, 40, 0, 0);
		AddNewRideContinuous("Bomb Jack", 2, 120, 1, -10, 40, 0, 0);
		AddNewRideContinuous("Tekken", 2, 120, 1, -20, 40, 0, 0);

		AddNewRideContinuous("Space Invaders", 2, 120, 1, -20, 50, 0, 0);
		AddNewRideContinuous("Powerdrift", 2, 120, 1, -10, 50, 0, 0);
		AddNewRideContinuous("Metal Slug", 2, 120, 1,   0, 50, 0, 0);
		AddNewRideContinuous("R-Type", 2, 120, 1, -10, 50, 0, 0);
		AddNewRideContinuous("Galaga", 2, 120, 1, -20, 50, 0, 0);
		
		break;

	default:
		std::cerr << "Invalid template value: " << park_template << std::endl;
	}

	UpdateThroughput();
}


int RideController::GetLargestValue(int parameter)
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


void RideController::OutputStatus(std::string status)
{
	if (ShowOutput)
	{
		std::cout << status << std::endl;
	}
}


// =================================================================================================================
// == Stats ========================================================================================================
// =================================================================================================================


void RideController::ShowStatistics(int park_open_hours)
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