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


#include <algorithm>
#include <string>

#include "Constants.h"
#include "MD5.h"
#include "QWaitTypes.h"
#include "Utility.h"


namespace Utility
{
	std::string DateTime(int mode)
	{
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);

		std::string year  = std::to_string(newtime.tm_year + 1900);
		std::string month = std::to_string(newtime.tm_mon + 1);
		std::string day   = std::to_string(newtime.tm_mday);
		std::string hour  = std::to_string(newtime.tm_hour);
		std::string min   = std::to_string(newtime.tm_min);
		std::string sec   = std::to_string(newtime.tm_sec);

		if (newtime.tm_mon < 10)
		{
			month = "0" + month;
		}

		if (newtime.tm_mday < 10)
		{
			month = "0" + month;
		}

		if (newtime.tm_hour < 10)
		{
			hour = "0" + hour;
		}

		if (newtime.tm_min < 10)
		{
			min = "0" + min;
		}

		if (newtime.tm_sec < 10)
		{
			sec = "0" + sec;
		}

		if (mode == kDisplayModeConsole)
		{
			return year + "/" + month + "/" + day + " " + hour + ":" + min + ":" + sec;
		}
		else
		{
			return year + month + day + "_" + hour + min + sec;
		}
	}


	std::string FormatTime(int hours, int minutes)
	{
		std::string h = std::to_string(hours);
		std::string m = std::to_string(minutes);

		if (hours < 10)
		{
			h = "0" + h;
		}

		if (minutes < 10)
		{
			m = "0" + m;
		}

		return h + ":" + m;
	}


	std::string FormatTime(QWaitTypes::Time time)
	{
		std::string h = std::to_string(time.hours);
		std::string m = std::to_string(time.minutes);

		if (time.hours < 10)
		{
			h = "0" + h;
		}

		if (time.minutes < 10)
		{
			m = "0" + m;
		}

		return h + ":" + m;
	}


	std::string PadRight(const std::string input, int pad)
	{
		if (input.length() < pad)
		{
			std::string output = input;
			
			for (int i = input.length(); i < pad; i++)
			{
				output = " " + output;
			}

			return input;
		}

		return input;
	}


	std::string PadRight(int input, int pad)
	{
		std::string output = std::to_string(input);

		if (output.length() < pad)
		{
			for (int i = output.length(); i < pad; i++)
			{
				output = " " + output;
			}

			return output;
		}

		return output;
	}


	// duration in minutes between From and To
	int Duration(QWaitTypes::Time from, QWaitTypes::Time to)
	{
		return static_cast<int>(((float)to.hours - (float)from.hours) * 60.0f + (to.minutes - from.minutes));
	}


	bool IsTimeEqual(QWaitTypes::Time base, QWaitTypes::Time compare_to)
	{
		if (base.hours == compare_to.hours && base.minutes == compare_to.minutes)
		{
			return true;
		}

		return false;
	}


	// returns true if CompareTo is between Ealiest and Latest (with a one hour margin of error)
	bool IsTicketValidForTime(QWaitTypes::Time earliest, QWaitTypes::Time latest, QWaitTypes::Time compare_to)
	{
		int e = (earliest.hours * 60) + earliest.minutes;
		int l = (latest.hours * 60) + latest.minutes;
		int q = (compare_to.hours * 60) + compare_to.minutes;
		int z = ((compare_to.hours + 1) * 60) + compare_to.minutes;

		if ((q >= e && q <= l) || (z >= e && z <= l))
		{
			return true;
		}

		return false;
	}


	std::string Utility::GetMD5(const std::string input)
	{
		std::string str(input.begin(), input.end());

		std::transform(str.begin(), str.end(), str.begin(), ::toupper);

		char* y = new char[str.length() + 1];

		strcpy_s(y, str.length() + 1, str.c_str());

		std::string MD5 = MD5::GetMD5HashString(y);

		delete[] y;

		return MD5.substr(0, MD5.size() - 2); // removes weird \n from end of string ?!
	}
};