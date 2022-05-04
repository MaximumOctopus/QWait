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


#include <fstream>
#include <iostream>
#include <string>
#include "Configuration.h"
#include "ReportHTML.h"
#include "RideController.h"
#include "VisitorController.h"


extern Configuration* GConfiguration;
extern RideController* GRideController;
extern VisitorController* GVisitorController;


namespace ReportHTML
{
	bool SaveSimulationReport(std::string filename, int fast_pass_mode)
	{
		std::ofstream file(filename);

		if (file)
		{
			file << "<html><head><title>" << GRideController->ParkName << "</title>\n";
			file << "<style type=\"text/css\">\n";
			file << "<!--\n";
			file << "A:link { color:#ff4400; text-decoration: none; }\n";
			file << "A:visited { color:#ff4400; text-decoration: none; }\n";
			file << "A:hover { color:#ff4400; text-decoration: underline; }\n";
			file << "A:active { color:#ff4400; text-decoration: none; }\n";
			file << ".ctable1 { font-family: Geneva, Arial, Helvetica, san-serif; font-size:10pt; color:#000000; }\n";
			file << "-- >\n";
			file << "</style></head><body>\n";

			// ==========================================================

			Summary(file);

			InsertSpacingTable(file);

			RideList(file);

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
		void Summary(std::ofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\"><td width=\"1211\" height=\"13\"><b>\"" << GRideController->ParkName << "\" Summary</b></td></tr>\n";
			
			if (GConfiguration->FastPassMode == Constants::FastPassModeNone)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\"><b>" << GRideController->Rides.size() << "</b> rides; theoretical throughput <b>" << GRideController->TheoreticalHourThroughputTotalDay << "</b> riders per day (actual: <b>" << GVisitorController->DailyStats.totalRides << "</b>); <b>" << GVisitorController->Visitors.size() << "</b> visitors; opening hours <b>" << GConfiguration->ParkOpenTime << ":00</b> to <b>" << GConfiguration->ParkCloseTime << ":00</b></td></tr>\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\"><b>" << GRideController->Rides.size() << "</b> rides; theoretical throughput <b>" << GRideController->TheoreticalHourThroughputTotalDay << "</b> riders per day (actual: <b>" << GVisitorController->DailyStats.totalRides << "</b>, fastpass rides " << GVisitorController->DailyStats.totalFastPastRides << "); <b>" << GVisitorController->Visitors.size() << "< / b> visitors; opening hours <b>" << GConfiguration->ParkOpenTime << ":00</b> to <b>" << GConfiguration->ParkCloseTime << ":00</b></td></tr>\n";
			}
			
			file << "<tr bgcolor=\"#" << kRowColour2 << "\"><td width=\"1211\" height=\"13\">Max rides <b>" << GVisitorController->DailyStats.maxRides << "</b>; Minimum rides <b>" << GVisitorController->DailyStats.minRides << "</b>; Zero rides <b>" << GVisitorController->DailyStats.zeroRides << "</b>; Average rides <b>" << GVisitorController->DailyStats.averageRides << "</b></tr>\n";
			file << "<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\">Distance travelled <b>" << GVisitorController->DailyStats.distanceTravelled << "</b> metres; Average distance travelled <b>" << GVisitorController->DailyStats.averageDistanceTravelled << "</b> metres</tr>\n";
			file << "<tr bgcolor=\"#" << kRowColour2 << "\"><td width=\"1211\" height=\"13\">Total idle time <b>" << GVisitorController->DailyStats.totalIdleTime << "</b> minutes; Total travel time <b>" << GVisitorController->DailyStats.totalTravellingTime << "</b> minutes; Total queue time <b>" << GVisitorController->DailyStats.totalQueueTime << "</b> minutes; Total ride time <b>" << GVisitorController->DailyStats.totalRidingTime << "</b> minutes"<< " </tr\n";
			file << "<tr bgcolor=\"#" << kRowColour1 << "\"><td width=\"1211\" height=\"13\">Average idle time <b>" << GVisitorController->DailyStats.averageIdleTime << "</b> minutes; Average travel time <b>" << GVisitorController->DailyStats.averageTravellingTime << "</b> minutes; Average queue time <b>" << GVisitorController->DailyStats.averageQueueTime << "</b> minutes; Average ride time <b>" << GVisitorController->DailyStats.averageRidingTime << "</b> minutes" << " </tr>\n";
			file << "<tr bgcolor=\"#" << kRowColour2 << "\"><td width=\"1211\" height=\"13\"></tr>\n";
			file << "</table>\n";
		}


		void RideList(std::ofstream& file)
		{
			if (GRideController->Rides.size() != 0)
			{
				// == ride throughput vs daily theoretical max ==============================================================================
				
				file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
				file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
				file << "<td width=\"400\" height=\"13\"><b>Name</b></td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\"><b>Rides</b></td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\"><b>Hourly Max.</b></td>\n";
				file << "<td width=\"400\" height=\"13\"><b>Throughput %</b></td>\n";
				file << "<td width=\"40\" height=\"13\" align=\"right\"><b>%</b></td>\n";
				file << "</tr>\n";

				for (int r = 0; r < GRideController->Rides.size(); r++)
				{
					if (r % 2 == 0)
					{
						file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
					}
					else
					{
						file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
					}

					float tpc = ((float)GRideController->Rides[r].DailyStatistics.totalRiders / ((float)GRideController->Rides[r].RideThroughput.totalPerHour * (float)GRideController->Rides[r].RideOperation.operationHours)) * 100.0f;

					file << "<td width=\"400\" height=\"13\">" << GRideController->Rides[r].RideOperation.name << "</td>\n";
					file << "<td width=\"80\" height=\"13\" align=\"right\">" << GRideController->Rides[r].DailyStatistics.totalRiders << "</td>\n";
					file << "<td width=\"80\" height=\"13\" align=\"right\">" << GRideController->Rides[r].RideThroughput.totalPerHour << "</td>\n";
					file << "<td width=\"400\" height=\"13\">" << GraphTable(400.0f, tpc, kGraphTypeGradient) << "</td>\n";
					file << "<td width=\"40\" height=\"13\" align=\"right\">" << static_cast<int>(tpc) << "%</td>\n";
					file << "</tr>\n";
				}

				file << "</table>\n";

				// == max queue length ======================================================================================================

				file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
				file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
				file << "<td width=\"400\" height=\"13\"><b>Name</b></td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\"><b>Max. Q</b></td>\n";
				file << "<td width=\"400\" height=\"13\"><b>Max. Queue</b></td>\n";
				file << "</tr>\n";

				int largestQueue = GRideController->GetLargestValue(kGetMaxQueueLength);

				for (int r = 0; r < GRideController->Rides.size(); r++)
				{
					if (r % 2 == 0)
					{
						file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
					}
					else
					{
						file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
					}

					float tpc = ((float)GRideController->Rides[r].DailyStatistics.maxQueueLength / (float)largestQueue) * 100.0f;

					file << "<td width=\"400\" height=\"13\">" << GRideController->Rides[r].RideOperation.name << "</td>\n";
					file << "<td width=\"80\" height=\"13\" align=\"right\">" << GRideController->Rides[r].DailyStatistics.maxQueueLength << "</td>\n";
					file << "<td width=\"400\" height=\"13\">" << GraphTable(400.0f, tpc, kGraphTypeGradient) << "</td>\n";
					file << "</tr>\n";
				}

				file << "</table>\n";
			}
		}


		void VisitorTypeList(std::ofstream& file)
		{
			if (GVisitorController->Visitors.size() != 0)
			{
				ThreeColumnTableHeader(file, "Type Description", "NRA", "No Ride Available");

				int largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeNoRideAvailable);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].noRideAvailable, ((float)GVisitorController->DailyStatsByType[t].noRideAvailable / (float)largest) * 100.0f);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, "Type Description", "WTTL", "Wait Time Too Long");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeWaitTimeTooLong);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].waitTimeTooLong, ((float)GVisitorController->DailyStatsByType[t].waitTimeTooLong / (float)largest) * 100.0f);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, "Type Description", "RS", "Ride Shutdown");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeRideShutdown);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].rideShutdown, ((float)GVisitorController->DailyStatsByType[t].rideShutdown / (float)largest) * 100.0f);
				}

				file << "</table>\n";

				ThreeColumnTableHeader(file, "Type Description", "DT (m)", "Distance Travelled (metres)");

				int total_travelled = 0;

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeDistanceTravelled);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].distanceTravelled, ((float)GVisitorController->DailyStatsByType[t].distanceTravelled / (float)largest) * 100.0f);

					total_travelled += GVisitorController->DailyStatsByType[t].distanceTravelled;
				}

				ThreeColumnTableRow(file, Constants::AvailableVisitorTypes + 1, "Total travelled", total_travelled, -1);

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageDistanceTravelled, ((float)GVisitorController->DailyStats.averageDistanceTravelled / (float)largest) * 100.0f);

				// ===

				ThreeColumnTableHeader(file, "Type Description", "AIT", "Average Idle Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageIdleTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].averageIdleTime, ((float)GVisitorController->DailyStatsByType[t].averageIdleTime / (float)largest) * 100.0f);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageIdleTime, ((float)GVisitorController->DailyStats.averageIdleTime / (float)largest) * 100.0f);

				ThreeColumnTableHeader(file, "Type Description", "ATT", "Average Travelling Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageTravellingTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].averageTravellingTime, ((float)GVisitorController->DailyStatsByType[t].averageTravellingTime / (float)largest) * 100.0f);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageTravellingTime), ((float)GVisitorController->DailyStats.averageTravellingTime / (float)largest) * 100.0f);

				ThreeColumnTableHeader(file, "Type Description", "AQT", "Average Queue Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageQueueTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], GVisitorController->DailyStatsByType[t].averageQueueTime, ((float)GVisitorController->DailyStatsByType[t].averageQueueTime / (float)largest) * 100.0f);
				}

				ThreeColumnTableEnd(file, true, GVisitorController->DailyStats.averageQueueTime, ((float)GVisitorController->DailyStats.averageQueueTime / (float)largest) * 100.0f);

				ThreeColumnTableHeader(file, "Type Description", "ART", "Average Riding Time (minutes)");

				largest = GVisitorController->GetLargestValueByType(kDailyStatsByTypeAverageRidingTime);

				for (int t = 0; t < Constants::AvailableVisitorTypes; t++)
				{
					ThreeColumnTableRow(file, t, Constants::TypeNames[t], static_cast<int>(GVisitorController->DailyStatsByType[t].averageRidingTime), ((float)GVisitorController->DailyStatsByType[t].averageRidingTime / (float)largest) * 100.0f);
				}

				ThreeColumnTableEnd(file, true, static_cast<int>(GVisitorController->DailyStats.averageRidingTime), ((float)GVisitorController->DailyStats.averageRidingTime / (float)largest) * 100.0f);
			}
		}


		void VisitorRideList(std::ofstream& file)
		{
			if (GVisitorController->Visitors.size() != 0)
			{
				FourColumnTableHeader(file, "Ride count", "# Visitors", "% of total", "%");

				int most_rides_index = GVisitorController->GetLargestValueByType(kDailyStatsHighestRideCountByIndex);
				int most_rides = GVisitorController->GetLargestValueByType(kDailyStatsHighestRideCountByValue);

				float pc_graph = 0;
				float pc_value = 0;

				for (int t = 0; t <= most_rides_index; t++)
				{
					pc_graph = ((float)GVisitorController->DailyStats.rideCount[t] / (float)most_rides) * 100.0f;
					pc_value = ((float)GVisitorController->DailyStats.rideCount[t] / (float)GVisitorController->Visitors.size()) * 100.0f;

					FourColumnTableRow(file, t, std::to_string(t), GVisitorController->DailyStats.rideCount[t], pc_graph, static_cast<int>(pc_value));
				}

				FourColumnTableEnd(file, true, GVisitorController->DailyStats.averageRides, 0);
			}
		}


		void MinuteByMinute(std::ofstream& file)
		{
			if (GConfiguration->FastPassMode == Constants::FastPassModeNone)
			{
				MxMTableHeader(file);
			}
			else
			{
				MxMTableHeaderFP(file);
			}

			for (int m = 0; m < GRideController->Rides[0].GetMinuteStatsCount(); m++)
			{
				MinuteDataV mdv = GVisitorController->GetMinuteDataStructFor(m);

				if (GConfiguration->FastPassMode == Constants::FastPassModeNone)
				{
					MxMTableRow(file, m, mdv.time, mdv.visitorsInPark, mdv.idle, mdv.travelling, mdv.queing, mdv.riding);
				}
				else
				{
					MxMTableRow(file, m, mdv.time, mdv.visitorsInPark, mdv.idle, mdv.travelling, mdv.queing, mdv.queingFastPass, mdv.riding);
				}
			}

			file << "</table>\n";
		}


		std::string GraphTable(float max, float pc, int colour_mode)
		{
			if (pc < 0.01f)
			{
				return "";
			}

			if (pc > 100.0f)
			{
				pc = 100.0f;
			}

			float width = (pc / 100.0f) * max;

			std::string colour = "#000000";

			switch (colour_mode)
			{
				case kGraphTypeGradient:
					if (pc < 75.0f)
					{
						colour = "#" + kGraphGreen;
					}
					else if (pc < 90.0f)
					{
						colour = "#" + kGraphYellow;
					}
					else
					{
						colour = "#" + kGraphRed;
					}
					break;
				case kGraphTypeSolidGreen:
					colour = "#" + kGraphGreen;
					break;
				case kGraphTypeAverage:
					colour = "#" + kGraphAverage;
					break;
			}

			return "<table width=\"" + std::to_string(static_cast<int>(width)) + "\" border=\"0\" bgcolor=\"" + colour + "\" height=\"8\" cellpadding=\"0\" cellspacing=\"0\"><tr><td></td></tr></table>";
		}


		void ThreeColumnTableHeader(std::ofstream& file, std::string col1, std::string col2, std::string col3)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"400\" height=\"13\"><b>" << col1 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << "<td width=\"720\" height=\"13\" align=\"left\"><b>" << col3 << "</b></td>\n";
			file << "</tr>\n";
		}


		void ThreeColumnTableRow(std::ofstream& file, int row, std::string col1, int col2, float graph_value)
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
			file << "<td width=\"720\" height=\"13\" align=\"left\">" << GraphTable(710.0f, graph_value, kGraphTypeGradient) << "</td>\n";
			file << "</tr>\n";
		}


		void ThreeColumnTableEnd(std::ofstream& file, bool show_average, int average, float graph_value)
		{
			if (show_average)
			{
				file << "<tr bgcolor=\"#" << kRowAverage << "\">\n";
				file << "<td width=\"400\" height=\"13\">Average</td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\">" << average << "</td>\n";
				file << "<td width=\"720\" height=\"13\" align=\"left\">" << GraphTable(710.0f, graph_value, kGraphTypeAverage) << "</td>\n";
				file << "</tr>\n";
			}

			file << "</table>\n";
		}


		void FourColumnTableHeader(std::ofstream& file, std::string col1, std::string col2, std::string col3, std::string col4)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"80\" height=\"13\"><b>" << col1 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << "<td width=\"990\" height=\"13\" align=\"left\"><b>" << col3 << "</b></td>\n";
			file << "<td width=\"50\" height=\"13\" align=\"right\"><b>" << col4 << "</b></td>\n";
			file << "</tr>\n";
		}


		void FourColumnTableEnd(std::ofstream& file, bool show_average, int average, float graph_value)
		{
			if (show_average)
			{
				file << "<tr bgcolor=\"#" << kRowAverage << "\">\n";
				file << "<td width=\"80\" height=\"13\">Average</td>\n";
				file << "<td width=\"80\" height=\"13\" align=\"right\">" << average << "</td>\n";
				file << "<td width=\"990\" height=\"13\" align=\"left\">" << GraphTable(980.0f, graph_value, kGraphTypeAverage) << "</td>\n";
				file << "<td width=\"50\" height=\"13\">" << graph_value << "%</td>\n";
			}

			file << "</tr>\n";
		}


		void FourColumnTableRow(std::ofstream& file, int row, std::string col1, int col2, float graph_value, int graph_percent)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td width=\"80\" height=\"13\" align=\"right\"><b>" << col1 << "</td>\n";
			file << "<td width=\"80\" height=\"13\" align=\"right\"><b>" << col2 << "</b></td>\n";
			file << "<td width=\"990\" height=\"13\" align=\"left\">" << GraphTable(980.0f, graph_value, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"50\" height=\"13\" align=\"right\"><b>" << graph_percent << "%</b></td>\n";
			file << "</tr>\n";
		}


		void MxMTableHeader(std::ofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"40\" height=\"13\" align=\"left\"><b>Time</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>In Park</b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Idle</b></td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Travelling</b></td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Queuing</b></td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Riding</b></td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "</tr>\n";
		}


		void MxMTableHeaderFP(std::ofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td width=\"40\" height=\"13\" align=\"left\"><b>Time</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>In Park</b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Idle</b></td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Travelling</b></td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Queuing</b></td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Queuing FP</b></td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"left\"><b>Riding</b></td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\"><b></b></td>\n";
			file << "</tr>\n";
		}


		void MxMTableRow(std::ofstream& file, int row, std::string time, int in_park, int idle, int travelling, int queuing, int riding)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td width=\"40\" height=\"13\" align=\"right\">" << time << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << in_park << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << idle << "</td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\">" << GraphTable(190.0f, ((float)(idle) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << travelling << "</td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\">" << GraphTable(190.0f, ((float)(travelling) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\">" << GraphTable(190.0f, ((float)(queuing) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << riding << "</td>\n";
			file << "<td width=\"190\" height=\"13\" align=\"left\">" << GraphTable(190.0f, ((float)(riding) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "</tr>\n";
		}


		// fastpass version
		void MxMTableRow(std::ofstream& file, int row, std::string time, int in_park, int idle, int travelling, int queuing, int queuing_fastpass, int riding)
		{
			if (row % 2 == 0)
			{
				file << "<tr bgcolor=\"#" << kRowColour1 << "\">\n";
			}
			else
			{
				file << "<tr bgcolor=\"#" << kRowColour2 << "\">\n";
			}

			file << "<td width=\"40\" height=\"13\" align=\"right\">" << time << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << in_park << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << idle << "</td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\">" << GraphTable(150.0f, ((float)(idle) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << travelling << "</td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\">" << GraphTable(150.0f, ((float)(travelling) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\">" << GraphTable(150.0f, ((float)(queuing) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << queuing << "</td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\">" << GraphTable(150.0f, ((float)(queuing_fastpass) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "<td width=\"60\" height=\"13\" align=\"right\">" << riding << "</td>\n";
			file << "<td width=\"150\" height=\"13\" align=\"left\">" << GraphTable(150.0f, ((float)(riding) / (float)(in_park)) * 100.0f, kGraphTypeGradient) << "</td>\n";
			file << "</tr>\n";
		}


		void Footer(std::ofstream& file)
		{
			file << "<table align=\"center\" width=\"1211\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\" class=\"ctable1\">\n";
			file << "<tr bgcolor=\"#" << kRowTitle << "\">\n";
			file << "<td height=\"13\" align=\"right\"><b>QWait</td>&nbsp;&nbsp;\n";
			file << "</tr></table></body></html>\n";
		}


		void InsertSpacingTable(std::ofstream& file)
		{
			file << "<table align=\"center\" width=\"770\" border=\"0\"><tr><td>&nbsp;</td></tr></table>\n";
		}
	}
}