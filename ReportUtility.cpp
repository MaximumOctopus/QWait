﻿// =======================================================================
//
// QWait, theme park modeller
//
// (c) Paul Alan Freshney 2022
// paul@freshney.org
//
// https://github.com/MaximumOctopus/QWait
// 
// =======================================================================


#include <string>

#include "ReportUtility.h"


namespace ReportUtility
{
	std::wstring BarGraphScale()
	{
		return L"|-------------- 100|";
	}


	std::wstring BarGraph(int percent)
	{
		int bar_count = 40;

		if (percent == 100)
		{

		}
		else
		{
			bar_count = static_cast<int>((double)percent / 2.5);
		}

		std::wstring bar = L"";
		bool add_half = false;

		if (bar_count % 2 != 0)
		{
			add_half = true;
		}

		bar_count = static_cast<int>((double)bar_count / 2.0);

		for (int t = 0; t < bar_count; t++)
		{
			bar += L"█";
		}

		if (add_half)
		{
			bar += L"▌";
		}

		return bar;
	}
}