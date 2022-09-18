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


#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "Configuration.h"
#include "ParkController.h"
#include "ReportHTML.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern ParkController* GParkController;
extern VisitorController* GVisitorController;


namespace ReportHTML
{
	bool SaveSimulationReport(const std::wstring file_name, FastPassType fast_pass_mode)
	{
		std::wofstream file(file_name);

		if (file)
		{
			file << L"<html><head><title>" << GParkController->entrance.ParkName << L"</title>\n";
			file << L"<style type=\"text/css\">\n";
			file << L"<!--\n";
			file << L"A:link { color:#ff4400; text-decoration: none; }\n";
			file << L"A:visited { color:#ff4400; text-decoration: none; }\n";
			file << L"A:hover { color:#ff4400; text-decoration: underline; }\n";
			file << L"A:active { color:#ff4400; text-decoration: none; }\n";
			file << L".ctable1 { font-family: Geneva, Arial, Helvetica, san-serif; font-size:10pt; color:#000000; }\n";
			file << L"-- >\n";
			file << L"</style></head><body>\n";

			// ==========================================================

			Summary(file);

			InsertSpacingTable(file);

			TicketList(file);

			InsertSpacingTable(file);

			RideList(file);

			InsertSpacingTable(file);

			if (GConfiguration->HandleFoodDrink)
			{
				EateryList(file);

				InsertSpacingTable(file);
			}

			GroupTypeList(file);

			InsertSpacingTable(file);

			VisitorRideList(file);

			InsertSpacingTable(file);

			VisitorTypeList(file);

			InsertSpacingTable(file);

			MinuteByMinute(file);

			InsertSpacingTable(file);

			Footer(file);

			// ==========================================================

			file.close();

			return true;
		}

		return false;
	}


	namespace
	{
		void Summary(std::wofstream& file)
		{
			file << std::fixed << std::setprecision(2);

			file << L"<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << L"<tr bgcolor=\"#" << kRowTitle << "\"><td width=\"1211\" height=\"13\" style=\"font-size:2em\"><b>\"" << GParkController->entrance.ParkName << "\" Summary</b></td></tr>\n";
			
			if (GConfiguration->FastPassMode == FastPassType::None)
			{
				file << L"<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\"><b>" << GParkController->Rides.size() << "</b> rides; theoretical throughput <b>" << GParkController->TheoreticalHourThroughputTotalDay << "</b> riders per day (actual: <b>" << GVisitorController->DailyStats.totalRides << "</b>); <b>" << GVisitorController->GroupCountCreated << "</b> groups; <b>" << GVisitorController->VisitorCountCreated << "</b> visitors; opening hours <b>" << GConfiguration->ParkOpenTime << ":00</b> to <b>" << GConfiguration->ParkCloseTime << ":00</b></td></tr>\n";
			}
			else
			{
				file << L"<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\"><b>" << GParkController->Rides.size() << "</b> rides; theoretical throughput <b>" << GParkController->TheoreticalHourThroughputTotalDay << "</b> riders per day (actual: <b>" << GVisitorController->DailyStats.totalRides << "</b>, fastpass rides " << GVisitorController->DailyStats.totalFastPastRides << "); <b>" << GVisitorController->GroupCountCreated << "</b> groups; <b>" << GVisitorController->VisitorCountCreated << "< / b> visitors; opening hours <b>" << GConfiguration->ParkOpenTime << ":00</b> to <b>" << GConfiguration->ParkCloseTime << ":00</b></td></tr>\n";
			}
			
			file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">Max rides <b>" << GVisitorController->DailyStats.maxRides << "</b>; Minimum rides <b>" << GVisitorController->DailyStats.minRides << "</b>; Zero rides <b>" << GVisitorController->DailyStats.zeroRides << "</b>; Average rides <b>" << GVisitorController->DailyStats.averageRides << "</b></tr>\n";
			file << L"<tr bgcolor=\"#" << kRowColour1 << L"\"><td width=\"1211\" height=\"13\">Distance travelled <b>" << GVisitorController->DailyStats.distanceTravelled << "</b> metres; Average distance travelled <b>" << GVisitorController->DailyStats.averageDistanceTravelled << "</b> metres</tr>\n";
			file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">Total idle time <b>" << GVisitorController->DailyStats.totalIdleTime << "</b> minutes; Total travel time <b>" << GVisitorController->DailyStats.totalTravellingTime << "</b> minutes; Total queue time <b>" << GVisitorController->DailyStats.totalQueueTime << "</b> minutes; Total ride time <b>" << GVisitorController->DailyStats.totalRidingTime << "</b> minutes"<< " </tr>\n";
			file << L"<tr bgcolor=\"#" << kRowColour1 << L"\"><td width=\"1211\" height=\"13\">Average idle time <b>" << GVisitorController->DailyStats.averageIdleTime << "</b> minutes; Average travel time <b>" << GVisitorController->DailyStats.averageTravellingTime << "</b> minutes; Average queue time <b>" << GVisitorController->DailyStats.averageQueueTime << "</b> minutes; Average ride time <b>" << GVisitorController->DailyStats.averageRidingTime << "</b> minutes" << " </tr>\n";
			file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">Total ticket spend <b>$" << GVisitorController->DailyStats.totalRideSpend << "</b>; average cost per ride <b>$" << GVisitorController->DailyStats.totalSpendPerRide << "</b></tr>\n";
			
			if (GConfiguration->HandleFoodDrink)
			{
				file << L"<tr bgcolor=\"#" << kRowColour1 << L"\"><td width=\"1211\" height=\"13\">Total eatery spend <b>$" << GVisitorController->DailyStats.totalEaterySpend << "</b>; average spend per visitor <b>$" << GVisitorController->DailyStats.averageEaterySpend << "</b></td></tr>\n";
				file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">Total eateries visited: <b>" << GVisitorController->DailyStats.totalEateriesVisited << "</b>; total travel food: <b>" << GVisitorController->DailyStats.totalTravellingFoodTime << "</b> mins; total queue food: <b>" << GVisitorController->DailyStats.totalQueueFoodTime << "</b> mins; total eat time <b>" << GVisitorController->DailyStats.totalEatingTime << "</b> mins</b></tr>\n";
				file << L"<tr bgcolor=\"#" << kRowColour1 << L"\"><td width=\"1211\" height=\"13\">Avg. travel food: <b>" << GVisitorController->DailyStats.averageTravellingFoodTime << "</b> mins; avg. queue food: <b>" << GVisitorController->DailyStats.averageQueueFoodTime << "</b> mins; avg. eat time <b>" << GVisitorController->DailyStats.averageEatingTime << "</b> mins</b></tr>\n";
				file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">Avg. eateries visited: <b>" << GVisitorController->DailyStats.averageEateriesVisited << "</b>; avg time per visit: <b>" << GVisitorController->DailyStats.averageTimePerEateryVisit << "</b> mins.</td></tr>\n";
			}

			file << L"<tr bgcolor=\"#" << kRowColour1 << L"\"><td width=\"1211\" height=\"13\">Temperature <b>" << GConfiguration->Climate.Temperature << "'C</b></td></tr>\n";
			file << L"<tr bgcolor=\"#" << kRowColour2 << L"\"><td width=\"1211\" height=\"13\">&nbsp;</td></tr>\n";
			file << L"</table>\n";
		}


		void TicketList(std::wofstream& file)
		{
			ThreeColumnTableHeader(file, L"Ticket purchases", L"Qty.", L"");

			int largest = GParkController->GetLargestTicketCategory();

			ThreeColumnTableRow(file, 0, L"Adult ($" + std::to_wstring(GParkController->entrance.Prices.Adult) + L")", GParkController->entrance.Tickets.Adult, ((double)GParkController->entrance.Tickets.Adult / (double)largest) * 100.0);
			ThreeColumnTableRow(file, 1, L"Child ($" + std::to_wstring(GParkController->entrance.Prices.Children) + L")", GParkController->entrance.Tickets.Children, ((double)GParkController->entrance.Tickets.Children / (double)largest) * 100.0);
			ThreeColumnTableRow(file, 2, L"Baby ($" + std::to_wstring(GParkController->entrance.Prices.Babies) + L")", GParkController->entrance.Tickets.Babies, ((double)GParkController->entrance.Tickets.Babies / (double)largest) * 100.0);

			ThreeColumnTableRow(file, 3, L"Adult advanced ($" + std::to_wstring(GParkController->entrance.Prices.AdultAdvance) + L")", GParkController->entrance.Tickets.AdultAdvance, ((double)GParkController->entrance.Tickets.AdultAdvance / (double)largest) * 100.0);
			ThreeColumnTableRow(file, 4, L"Child advanced ($" + std::to_wstring(GParkController->entrance.Prices.ChildrenAdvance) + L")", GParkController->entrance.Tickets.ChildrenAdvance, ((double)GParkController->entrance.Tickets.ChildrenAdvance / (double)largest) * 100.0);
			ThreeColumnTableRow(file, 5, L"Baby advanced ($" + std::to_wstring(GParkController->entrance.Prices.BabiesAdvance) + L")", GParkController->entrance.Tickets.BabiesAdvance, ((double)GParkController->entrance.Tickets.BabiesAdvance / (double)largest) * 100.0);

			file << "</table>\n";
		}


		void RideList(std::wofstream& file)
		{
			if (GParkController->Rides.size() != 0)
			{
				// == ride throughput vs daily theoretical max ==============================================================================
				
				FiveColumnHeader(file, L"Name", L"Rides", L"Hourly Max.", L"Throughput %");

				for (int r = 0; r < GParkController->Rides.size(); r++)
				{
					if (r % 2 == 0)
					{
						file << L"<tr bgcolor=\"#" << kRowColour1 << L"\">\n";
					}
					else
					{
						file << L"<tr bgcolor=\"#" << kRowColour2 << L"\">\n";
					}

					double tpc = ((double)GParkController->Rides[r].DailyStatistics.totalRiders / ((double)GParkController->Rides[r].RideThroughput.totalPerHour * (double)GParkController->Rides[r].RideOperation.operationHours)) * 100.0;

					FiveColumnTableRow(file, r, GParkController->Rides[r].RideOperation.name, GParkController->Rides[r].DailyStatistics.totalRiders, GParkController->Rides[r].RideThroughput.totalPerHour, tpc, static_cast<int>(tpc));
				}

				file << "</table>\n";

				InsertSpacingTable(file);

				// == max queue length ======================================================================================================

				ThreeColumnTableHeader(file, L"Name", L"Max. Q", L"Max. Queue (visitors)");

				int largestQueue = GParkController->GetLargestValue(kGetMaxQueueLength);

				for (int r = 0; r < GParkController->Rides.size(); r++)
				{
					ThreeColumnTableRow(file, r, GParkController->Rides[r].RideOperation.name, GParkController->Rides[r].DailyStatistics.maxQueueLength, ((double)GParkController->Rides[r].DailyStatistics.maxQueueLength / (double)largestQueue) * 100.0);
				}

				file << "</table>\n";
			}
		}


		void EateryList(std::wofstream& file)
		{
			if (GParkController->Eateries.size() != 0)
			{
				FiveColumnAltHeader(file, L"Name", L"Customers", L"Max Q.", L"Q", L"Income");

				int largestQueue = GParkController->GetLargestValue(kGetMaxQueueLength);

				for (int e = 0; e < GParkController->Eateries.size(); e++)
				{
					double tpc = ((double)GParkController->Eateries[e].Statistics.MaxQueue / (double)largestQueue) * 100;

					FiveColumnAltTableRow(file, e, GParkController->Eateries[e].Name, GParkController->Eateries[e].Statistics.Customers, GParkController->Eateries[e].Statistics.MaxQueue, tpc, GParkController->Eateries[e].Statistics.Spend);
				}

				file << "</table>\n";
			}			
		}


		void GroupTypeList(std::wofstream& file)
		{
			if (GVisitorController->GroupCountCreated != 0)
			{
				ThreeColumnTableHeader(file, L"Group Description", L"NRA", L"No Ride Available");

				int largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeNoRideAvailable);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], GVisitorController->DailyStatsByGroup[t].noRideAvailable, ((double)GVisitorController->DailyStatsByGroup[t].noRideAvailable / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Group Description", L"WTTL", L"Wait Time Too Long");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeWaitTimeTooLong);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], GVisitorController->DailyStatsByGroup[t].waitTimeTooLong, ((double)GVisitorController->DailyStatsByGroup[t].waitTimeTooLong / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Group Description", L"RS", L"Ride Shutdown");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeRideShutdown);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], GVisitorController->DailyStatsByGroup[t].rideShutdown, ((double)GVisitorController->DailyStatsByGroup[t].rideShutdown / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Group Description", L"EQTL", L"Eatery Queue Too Long");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeEateryQueueTooLong);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], GVisitorController->DailyStatsByGroup[t].eateryQueueTooLong, ((double)GVisitorController->DailyStatsByGroup[t].eateryQueueTooLong / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Group Description", L"DT (m)", L"Distance Travelled (metres)");

				int total_travelled = 0;

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeDistanceTravelled);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], GVisitorController->DailyStatsByGroup[t].distanceTravelled, ((double)GVisitorController->DailyStatsByGroup[t].distanceTravelled / (double)largest) * 100.0);

					total_travelled += GVisitorController->DailyStatsByGroup[t].distanceTravelled;
				}

				ThreeColumnTableRow(file, Constants::AvailableVisitorTypes + 1, L"Total travelled", total_travelled, -1);

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageDistanceTravelled, ((double)GVisitorController->DailyStats.averageDistanceTravelled / (double)largest) * 100.0);

				// ===

				ThreeColumnTableHeader(file, L"Group Description", L"AIT", L"Average Idle Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageIdleTime);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					int ait = static_cast<int>(GVisitorController->DailyStatsByGroup[t].averageIdleTime);

					ThreeColumnTableRow(file, t, Constants::GroupNames[t], ait, ((double)GVisitorController->DailyStatsByGroup[t].averageIdleTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageIdleTime, ((double)GVisitorController->DailyStats.averageIdleTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Group Description", L"ATT", L"Average Travelling Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageTravellingTime);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					int att = static_cast<int>(GVisitorController->DailyStatsByGroup[t].averageTravellingTime);

					ThreeColumnTableRow(file, t, Constants::GroupNames[t], att, ((double)GVisitorController->DailyStatsByGroup[t].averageTravellingTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageTravellingTime), ((double)GVisitorController->DailyStats.averageTravellingTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Group Description", L"AQT", L"Average Queue Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageQueueTime);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					int aqt = static_cast<int>(GVisitorController->DailyStatsByGroup[t].averageQueueTime);

					ThreeColumnTableRow(file, t, Constants::GroupNames[t], aqt, ((double)GVisitorController->DailyStatsByGroup[t].averageQueueTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageQueueTime, ((double)GVisitorController->DailyStats.averageQueueTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Group Description", L"ART", L"Average Riding Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageRidingTime);

				for (int t = 0; t < Constants::AvailableGroupTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::GroupNames[t], static_cast<int>(GVisitorController->DailyStatsByGroup[t].averageRidingTime), ((double)GVisitorController->DailyStatsByGroup[t].averageRidingTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageRidingTime), ((double)GVisitorController->DailyStats.averageRidingTime / (double)largest) * 100.0);
			}
		}


		void VisitorTypeList(std::wofstream& file)
		{
			if (GVisitorController->GroupCountCreated != 0)
			{
				ThreeColumnTableHeader(file, L"Type Description", L"NRA", L"No Ride Available");

				int largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeNoRideAvailable);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].noRideAvailable, ((double)GVisitorController->DailyStatsByType[t].noRideAvailable / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Type Description", L"WTTL", L"Wait Time Too Long");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeWaitTimeTooLong);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].waitTimeTooLong, ((double)GVisitorController->DailyStatsByType[t].waitTimeTooLong / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Type Description", L"RS", L"Ride Shutdown");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeRideShutdown);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].rideShutdown, ((double)GVisitorController->DailyStatsByType[t].rideShutdown / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Type Description", L"EQTL", L"Eatery Queue Too Long");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeEateryQueueTooLong);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].eateryQueueTooLong, ((double)GVisitorController->DailyStatsByType[t].eateryQueueTooLong / (double)largest) * 100.0);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, L"Type Description", L"DT (m)", L"Distance Travelled (metres)");

				int total_travelled = 0;

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeDistanceTravelled);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].distanceTravelled, ((double)GVisitorController->DailyStatsByType[t].distanceTravelled / (double)largest) * 100.0);

					total_travelled += GVisitorController->DailyStatsByType[t].distanceTravelled;
				}

				ThreeColumnTableRow(file, Constants::AvailableVisitorTypes + 1, L"Total travelled", total_travelled, -1);

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageDistanceTravelled, ((double)GVisitorController->DailyStats.averageDistanceTravelled / (double)largest) * 100.0);

				// ===

				ThreeColumnTableHeader(file, L"Type Description", L"AIT", L"Average Idle Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageIdleTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].averageIdleTime, ((double)GVisitorController->DailyStatsByType[t].averageIdleTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageIdleTime, ((double)GVisitorController->DailyStats.averageIdleTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Type Description", L"ATT", L"Average Travelling Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageTravellingTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].averageTravellingTime, ((double)GVisitorController->DailyStatsByType[t].averageTravellingTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageTravellingTime), ((double)GVisitorController->DailyStats.averageTravellingTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Type Description", L"AQT", L"Average Queue Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageQueueTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), GVisitorController->DailyStatsByType[t].averageQueueTime, ((double)GVisitorController->DailyStatsByType[t].averageQueueTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageQueueTime, ((double)GVisitorController->DailyStats.averageQueueTime / (double)largest) * 100.0);

				ThreeColumnTableHeader(file, L"Type Description", L"ART", L"Average Riding Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageRidingTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, QWaitTypes::VisitorTypeToString(t), static_cast<int>(GVisitorController->DailyStatsByType[t].averageRidingTime), ((double)GVisitorController->DailyStatsByType[t].averageRidingTime / (double)largest) * 100.0);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageRidingTime), ((double)GVisitorController->DailyStats.averageRidingTime / (double)largest) * 100.0);
			}
		}


		void VisitorRideList(std::wofstream& file)
		{
			if (GVisitorController->GroupCountCreated != 0)
			{
				FourColumnTableHeader(file, L"Ride count", L"# Visitors", L"% of total", L"%");

				int most_rides_index = GVisitorController->GetLargestValueByType(kDailyStatsHighestRideCountByIndex);
				int most_rides = GVisitorController->GetLargestValueByType(kDailyStatsHighestRideCountByValue);

				double pc_graph = 0;
				double pc_value = 0;

				for (int t = 0; t <= most_rides_index; t++)
				{
					pc_graph = ((double)GVisitorController->DailyStats.rideCount[t] / (double)most_rides) * 100.0;
					pc_value = ((double)GVisitorController->DailyStats.rideCount[t] / (double)GVisitorController->VisitorCountCreated) * 100.0;

					FourColumnTableRow(file, t, std::to_wstring(t), GVisitorController->DailyStats.rideCount[t], pc_graph, static_cast<int>(pc_value));
				}

				FourColumnTableEnd(file, true, GVisitorController->DailyStats.averageRides, 0);
			}
		}


		void MinuteByMinute(std::wofstream& file)
		{
			if (GConfiguration->FastPassMode == FastPassType::None)
			{
				MxMTableHeader(file);
			}
			else
			{
				MxMTableHeaderFP(file);
			}

			for (int m = 0; m < GParkController->Rides[0].GetMinuteStatsCount(); m++)
			{
				MinuteDataV mdv = GVisitorController->GetMinuteDataStructFor(m);

				if (GConfiguration->FastPassMode == FastPassType::None)
				{
					MxMTableRow(file, m, mdv.time, mdv.visitorsInPark, mdv.idle, mdv.travelling, mdv.queuing, mdv.riding, mdv.waiting);
				}
				else
				{
					MxMTableRow(file, m, mdv.time, mdv.visitorsInPark, mdv.idle, mdv.travelling, mdv.queuing, mdv.queuingFastPass, mdv.riding, mdv.waiting);
				}
			}

			file << "</table>\n";
		}


		std::wstring GraphTable(double max, double pc, int colour_mode)
		{
			if (pc < 0.01)
			{
				return L"";
			}

			if (pc > 100.0)
			{
				pc = 100.0;
			}

			double width = (pc / 100.0) * max;

			std::wstring colour = L"#000000";

			switch (colour_mode)
			{
				case kGraphTypeGradient:
					if (pc < 75.0)
					{
						colour = L"#" + kGraphGreen;
					}
					else if (pc < 90.0)
					{
						colour = L"#" + kGraphYellow;
					}
					else
					{
						colour = L"#" + kGraphRed;
					}
					break;
				case kGraphTypeSolidGreen:
					colour = L"#" + kGraphGreen;
					break;
				case kGraphTypeAverage:
					colour = L"#" + kGraphAverage;
					break;
			}

			if (width <= 0)
			{
				return L"&nbsp;";
			}

			return L"<table width=\"" + std::to_wstring(static_cast<int>(width)) + L"\" border=\"0\" bgcolor=\"" + colour + L"\" height=\"8\" cellpadding=\"0\" cellspacing=\"0\"><tr><td></td></tr></table>";
		}


		void ThreeColumnTableHeader(std::wofstream& file, std::wstring col1, std::wstring col2, std::wstring col3)
		{
			file << L"<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << L"<tr bgcolor=\"#" << kRowTitle << L"\">\n";
			file << L"<td width=\"400\" height=\"13\"><b>" << col1 << L"</td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << L"</b></td>\n";
			file << L"<td width=\"720\" height=\"13\" align=\"left\"><b>" << col3 << L"</b></td>\n";
			file << L"</tr>\n";
		}


		void ThreeColumnTableRow(std::wofstream& file, int row, std::wstring col1, int col2, double graph_value)
		{
			if (row % 2 == 0)
			{
				file << L"<tr bgcolor=\"#" << kRowColour1 << L"\">\n";
			}
			else
			{
				file << L"<tr bgcolor=\"#" << kRowColour2 << L"\">\n";
			}

			file << L"<td width=\"400\" height=\"13\">" << col1 << L"</td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\">" << col2 << L"</td>\n";
			file << L"<td width=\"720\" height=\"13\" align=\"left\">" << GraphTable(710.0, graph_value, kGraphTypeGradient) << "</td>\n";
			file << L"</tr>\n";
		}


		void ThreeColumnTableEnd(std::wofstream& file, bool show_average, int average, double graph_value)
		{
			if (show_average)
			{
				file << L"<tr bgcolor=\"#" << kRowAverage << "\">\n";
				file << L"<td width=\"400\" height=\"13\">Average</td>\n";
				file << L"<td width=\"80\" height=\"13\" align=\"right\">" << average << L"</td>\n";
				file << L"<td width=\"720\" height=\"13\" align=\"left\">" << GraphTable(710.0, graph_value, kGraphTypeAverage) << "</td>\n";
				file << L"</tr>\n";
			}

			file << "</table>\n";
		}


		void FourColumnTableHeader(std::wofstream& file, std::wstring col1, std::wstring col2, std::wstring col3, std::wstring col4)
		{
			file << L"<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << L"<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << L"<td width=\"80\" height=\"13\"><b>" << col1 << "</td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << L"<td width=\"990\" height=\"13\" align=\"left\"><b>" << col3 << "</b></td>\n";
			file << L"<td width=\"50\" height=\"13\" align=\"right\"><b>" << col4 << "</b></td>\n";
			file << L"</tr>\n";
		}


		void FourColumnTableEnd(std::wofstream& file, bool show_average, int average, double graph_value)
		{
			if (show_average)
			{
				file << "<tr bgcolor=\"#" << kRowAverage << "\">\n";
				file << "<td width=\"80\" height=\"13\">Average</td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\">" << average << "</td>\n";
				file << "<td width=\"990\" height=\"13\" align=\"left\">" << GraphTable(980.0, graph_value, kGraphTypeAverage) << "</td>\n";
				file << "<td width=\"50\" height=\"13\">" << graph_value << "%</td>\n";
			}

			file << "</tr>\n";
		}


		void FourColumnTableRow(std::wofstream& file, int row, std::wstring col1, int col2, double graph_value, int graph_percent)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col1 << "</td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << L"<td width=\"990\" height=\"13\" align=\"left\">" << GraphTable(980.0, graph_value, kGraphTypeGradient) << "</td>\n";
			file << L"<td width=\"50\" height=\"13\" align=\"right\"><b>" << graph_percent << "%</b></td>\n";
			file << L"</tr>\n";
		}


		void FiveColumnHeader(std::wofstream& file, std::wstring col1, std::wstring col2, std::wstring col3, std::wstring col4)
		{
			file << L"<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << L"<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"400\" height=\"13\"><b>" << col1 << "</b></td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col3 << "</b></td>\n";
			file << L"<td width=\"400\" height=\"13\"><b>" << col4 << "</b></td>\n";
			file << L"<td width=\"40\" height=\"13\" align=\"right\"><b>%</b></td>\n";
			file << L"</tr>\n";
		}


		void FiveColumnTableRow(std::wofstream& file, int row, std::wstring col1, int col2, int col3, double graph_value, int graph_percent)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td width=\"400\" height=\"13\">" << col1 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\">" << col2 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\">" << col3 << "</td>\n";
			file << "<td width=\"400\" height=\"13\">" << GraphTable(400.0, graph_value, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"40\" height=\"13\" align=\"right\">" << graph_percent << "%</td>\n";
			file << "</tr>\n";
		}


		void FiveColumnAltHeader(std::wofstream& file, std::wstring col1, std::wstring col2, std::wstring col3, std::wstring col4, std::wstring col5)
		{
			file << L"<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << L"<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"400\" height=\"13\"><b>" << col1 << "</b></td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << L"<td width=\"80\" height=\"13\" align=\"right\"><b>" << col3 << "</b></td>\n";
			file << L"<td width=\"400\" height=\"13\"><b>" << col4 << "</b></td>\n";
			file << L"<td width=\"40\" height=\"13\" align=\"right\"><b>" << col5 << "</b></td>\n";
			file << L"</tr>\n";
		}


		void FiveColumnAltTableRow(std::wofstream& file, int row, std::wstring col1, int col2, int col3, double graph_value, int col5)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td width=\"400\" height=\"13\">" << col1 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\">" << col2 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\">" << col3 << "</td>\n";
			file << "<td width=\"400\" height=\"13\">" << GraphTable(400.0, graph_value, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"40\" height=\"13\" align=\"right\">" << col5 << "</td>\n";
			file << "</tr>\n";
		}


		void MxMTableHeader(std::wofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"36\" height=\"13\" align=\"left\"><b>Time</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>In Park</b></td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Idle</b></td>\n";
			file << "<td width=\"158\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Travel</b></td>\n";
			file << "<td width=\"158\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Queue</b></td>\n";
			file << "<td width=\"158\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Ride</b></td>\n";
			file << "<td width=\"158\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Busy</b></td>\n";
			file << "<td width=\"158\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "</tr>\n";
		}


		void MxMTableHeaderFP(std::wofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"45\" height=\"13\" align=\"left\"><b>Time</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>In Park</b></td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Idle</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Travel</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Queue</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Queue FP</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Ride</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "<td width=\"55\" height=\"13\" align=\"left\"><b>Busy</b></td>\n";
			file << "<td width=\"121\" height=\"13\" align=\"left\">&nbsp;</td>\n";
			file << "</tr>\n";
		}


		void MxMTableRow(std::wofstream& file, int row, std::wstring time, int in_park, int idle, int travelling, int queuing, int riding, int busy)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td height=\"13\" align=\"right\">" << time << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << in_park << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << idle << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(155.0, ((double)(idle) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << travelling << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(155.0, ((double)(travelling) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(155.0, ((double)(queuing) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << riding << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(155.0, ((double)(riding) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << busy << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(155.0, ((double)(busy) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "</tr>\n";
		}


		// fastpass version
		void MxMTableRow(std::wofstream& file, int row, std::wstring time, int in_park, int idle, int travelling, int queuing, int queuing_fastpass, int riding, int busy)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td height=\"13\" align=\"right\">" << time << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << in_park << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << idle << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(idle) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << travelling << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(travelling) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(queuing) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(queuing_fastpass) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << riding << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(riding) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "<td height=\"13\" align=\"right\">" << busy << "</td>\n";
			file << "<td height=\"13\" align=\"left\">" << GraphTable(119.0, ((double)(busy) / (double)(in_park)) * 100.0, kGraphTypeGradient) << "</td>\n";
			file << "</tr>\n";
		}


		void Footer(std::wofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td height=\"13\" align=\"right\"><b>QWait</td>&nbsp;&nbsp;\n";
			file << "</tr></table></body></html>\n";
		}


		void InsertSpacingTable(std::wofstream& file)
		{
			file << "<table align=\"center\" width=\"770\" border=\"0\"><tr><td>&nbsp;</td></tr></table>\n";
		}
	}
}