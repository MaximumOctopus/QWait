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


#include <fstream>


static const int kGraphTypeGradient = 0;
static const int kGraphTypeSolidGreen = 1;
static const int kGraphTypeAverage = 2;


static const std::string kRowTitle = "88DDFF";
static const std::string kRowColour1 = "EEEEEE";
static const std::string kRowColour2 = "DDDDDD";
static const std::string kRowAverage = "EEEEFF";

static const std::string kGraphGreen = "00BB22";
static const std::string kGraphYellow = "BBBB22";
static const std::string kGraphRed = "FF0000";
static const std::string kGraphAverage = "FF0000";


namespace ReportHTML
{
	bool SaveSimulationReport(const std::string, FastPassType);

	namespace
	{
		void Summary(std::ofstream&);

		void TicketList(std::ofstream&);
		void RideList(std::ofstream&);
		void GroupTypeList(std::ofstream&);
		void VisitorTypeList(std::ofstream&);
		void VisitorRideList(std::ofstream&);
		void MinuteByMinute(std::ofstream&);

		void ThreeColumnTableHeader(std::ofstream&, std::string, std::string, std::string);
		void ThreeColumnTableRow(std::ofstream&, int, std::string, int, float);
		void ThreeColumnTableEnd(std::ofstream&, bool, int, float);

		void FourColumnTableHeader(std::ofstream&, std::string, std::string, std::string, std::string);
		void FourColumnTableRow(std::ofstream&, int, std::string, int, float, int);
		void FourColumnTableEnd(std::ofstream&, bool, int, float);

		void FiveColumnHeader(std::ofstream&, std::string, std::string, std::string, std::string);
		void FiveColumnTableRow(std::ofstream&, int, std::string, int, int, float, int);

		void MxMTableHeader(std::ofstream&);
		void MxMTableHeaderFP(std::ofstream&);
		void MxMTableRow(std::ofstream&, int, std::string, int, int, int, int, int, int);
		void MxMTableRow(std::ofstream&, int, std::string, int, int, int, int, int, int, int);

		std::string GraphTable(float, float, int);

		void Footer(std::ofstream&);

		void InsertSpacingTable(std::ofstream&);
	}
};