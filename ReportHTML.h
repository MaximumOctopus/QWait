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


static const std::wstring kRowTitle = L"88DDFF";
static const std::wstring kRowColour1 = L"EEEEEE";
static const std::wstring kRowColour2 = L"DDDDDD";
static const std::wstring kRowAverage = L"EEEEFF";

static const std::wstring kGraphGreen = L"00BB22";
static const std::wstring kGraphYellow = L"BBBB22";
static const std::wstring kGraphRed = L"FF0000";
static const std::wstring kGraphAverage = L"FF0000";


namespace ReportHTML
{
	bool SaveSimulationReport(const std::wstring, FastPassType);

	namespace
	{
		void Summary(std::wofstream&);

		void TicketList(std::wofstream&);
		void RideList(std::wofstream&);
		void GroupTypeList(std::wofstream&);
		void VisitorTypeList(std::wofstream&);
		void VisitorRideList(std::wofstream&);
		void MinuteByMinute(std::wofstream&);

		void ThreeColumnTableHeader(std::wofstream&, std::wstring, std::wstring, std::wstring);
		void ThreeColumnTableRow(std::wofstream&, int, std::wstring, int, double);
		void ThreeColumnTableEnd(std::wofstream&, bool, int, double);

		void FourColumnTableHeader(std::wofstream&, std::wstring, std::wstring, std::wstring, std::wstring);
		void FourColumnTableRow(std::wofstream&, int, std::wstring, int, double, int);
		void FourColumnTableEnd(std::wofstream&, bool, int, double);

		void FiveColumnHeader(std::wofstream&, std::wstring, std::wstring, std::wstring, std::wstring);
		void FiveColumnTableRow(std::wofstream&, int, std::wstring, int, int, double, int);

		void MxMTableHeader(std::wofstream&);
		void MxMTableHeaderFP(std::wofstream&);
		void MxMTableRow(std::wofstream&, int, std::wstring, int, int, int, int, int, int);
		void MxMTableRow(std::wofstream&, int, std::wstring, int, int, int, int, int, int, int);

		std::wstring GraphTable(double, double, int);

		void Footer(std::wofstream&);

		void InsertSpacingTable(std::wofstream&);
	}
};