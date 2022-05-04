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
}