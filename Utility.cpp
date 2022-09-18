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
#include <codecvt>
#include <locale>
#include <string>

#include "Constants.h"
#include "MD5.h"
#include "QWaitTypes.h"
#include "Utility.h"


namespace Utility
{
	std::wstring DateTime(int mode)
	{
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);

		std::wstring year  = std::to_wstring(newtime.tm_year + 1900);
		std::wstring month = std::to_wstring(newtime.tm_mon + 1);
		std::wstring day   = std::to_wstring(newtime.tm_mday);
		std::wstring hour  = std::to_wstring(newtime.tm_hour);
		std::wstring min   = std::to_wstring(newtime.tm_min);
		std::wstring sec   = std::to_wstring(newtime.tm_sec);

		if (newtime.tm_mon < 10)
		{
			month = L"0" + month;
		}

		if (newtime.tm_mday < 10)
		{
			month = L"0" + month;
		}

		if (newtime.tm_hour < 10)
		{
			hour = L"0" + hour;
		}

		if (newtime.tm_min < 10)
		{
			min = L"0" + min;
		}

		if (newtime.tm_sec < 10)
		{
			sec = L"0" + sec;
		}

		if (mode == kDisplayModeConsole)
		{
			return year + L"/" + month + L"/" + day + L" " + hour + L":" + min + L":" + sec;
		}
		else
		{
			return year + month + day + L"_" + hour + min + sec;
		}
	}


	std::wstring FormatTime(int hours, int minutes)
	{
		std::wstring h = std::to_wstring(hours);
		std::wstring m = std::to_wstring(minutes);

		if (hours < 10)
		{
			h = L"0" + h;
		}

		if (minutes < 10)
		{
			m = L"0" + m;
		}

		return h + L":" + m;
	}


	std::wstring FormatTime(QWaitTypes::Time time)
	{
		std::wstring h = std::to_wstring(time.hours);
		std::wstring m = std::to_wstring(time.minutes);

		if (time.hours < 10)
		{
			h = L"0" + h;
		}

		if (time.minutes < 10)
		{
			m = L"0" + m;
		}

		return h + L":" + m;
	}


	std::wstring PadRight(const std::wstring input, int pad)
	{
		if (input.length() < pad)
		{
			std::wstring output = input;
			
			for (int i = input.length(); i < pad; i++)
			{
				output = L" " + output;
			}

			return output;
		}

		return input;
	}


	std::wstring PadRight(int input, int pad)
	{
		std::wstring output = std::to_wstring(input);

		if (output.length() < pad)
		{
			for (int i = output.length(); i < pad; i++)
			{
				output = L" " + output;
			}

			return output;
		}

		return output;
	}


	// duration in minutes between From and To
	int Duration(QWaitTypes::Time from, QWaitTypes::Time to)
	{
		return static_cast<int>(((double)to.hours - (double)from.hours) * 60.0 + (to.minutes - from.minutes));
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


	std::wstring Utility::GetMD5(const std::wstring input)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string str = converter.to_bytes(input);

		std::transform(str.begin(), str.end(), str.begin(), ::toupper);

		char* y = new char[str.length() + 1];

		strcpy_s(y, str.length(), str.c_str());

		std::wstring MD5 = MD5::GetMD5HashString(y);

		delete[] y;

		return MD5.substr(0, MD5.size() - 2); // removes weird \n from end of string ?!
	}
};