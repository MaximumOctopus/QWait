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


#include <iostream>
#include "Constants.h"
#include "Help.h"


namespace Help
{
	void ShowHelp()
	{
        std::wcout << "\n   QWait " << Constants::QWVersion << " :: " << Constants::QWDate << "\n";
        std::wcout << L"      (c) Paul Alan Freshney 2022\n";
        std::wcout << "\n";
        std::wcout << L"          https://qwait.sourceforge.io :: paul@freshney.org\n";
        std::wcout << "\n";
        std::wcout << L"  Use \"QWait /?\" for this help.\n";
        std::wcout << "\n";
        std::wcout << L"    /Visitors;x                 set visitor count.\n";
        std::wcout << L"    /Template;x                 where is x is the template to use:\n";
        std::wcout << L"                                    0 : test park\n";
        std::wcout << L"                                    1 : Animal Kingom\n";
        std::wcout << L"                                    2 : EPCOT\n";
        std::wcout << L"                                    3 : Hollywood Studios\n";
        std::wcout << L"                                    4 : Magic Kingdom\n";
        std::wcout << L"                                    5 : Coin-op arcade\n";
        std::wcout << L"                                    6 : Alton Towers UK\n";
        std::wcout << L"    /UseConfigFile;filename     set configuration from specified config file.\n";
        std::wcout << "\n";
        std::wcout << L"    /ImportVisitorDemo;filename rather than generating random visitors, the data from.\n";
        std::wcout << L"                                the specified file is used. Companion of /ReportVisitorDemo.\n";
        std::wcout << L"                                ignores the value set from /Visitors;x\n";
        std::wcout << "\n";
        std::wcout << L"    /FoodDrink                  enabled visitors' food/drink requirements.\n";
        std::wcout << "\n";
        std::wcout << L"    /DebugSCC                   export selection choice cache.\n";
        std::wcout << L"                                this is the data used by the visitor types to pick rides.\n";
        std::wcout << L"    /DebugDistanceCache         export the time and distance caches to a csv file.\n";
        std::wcout << L"                                a table of ride->ride values.\n";
        std::wcout << L"    /DebugNoExecute             everything except running the model.\n";
        std::wcout << L"    /DebugUpdateRate;x          updated statistics every x minutes to the console.\n";
        std::wcout << L"                                default is 15\n";
        std::wcout << "\n";
        std::wcout << L"    /ReportHTML;filename        export HTML simulation summary report.\n";
        std::wcout << "\n";
        std::wcout << L"    /ReportCVD                  export complete visitor data.\n";
        std::wcout << L"    /ReportCSVmxm               export a minute-by-minute report in CSV format.\n";
        std::wcout << L"    /ReportTextmxm              export a minute-by-minute report as a text file.\n";
        std::wcout << "\n";
        std::wcout << L"    /ReportSimulation           important information regarding the last sim.\n";
        std::wcout << L"    /ReportVisitorLocation      exports complete minute by minute report.\n";
        std::wcout << L"                                of visitor location. Warning! This generates\n";
        std::wcout << L"                                very large output: ~280MB per 20000 visitors!\n";
        std::wcout << L"    /ReportVisitorDemo;filename export visitor demographics (useful if you want\n";
        std::wcout << L"                                to maintain a consistent set of visitors across\n";
        std::wcout << L"                                multuple runs).\n";
        std::wcout << L"    /ReportVisitorRideList      export complete list of visitors and the rides\n";
        std::wcout << L"                                ridden today (csv).\n";
        std::wcout << std::endl;
	}


    // all of my software is now free and open source. 
    // please donate to your local cat charity or shelter.
    void ShowCat()
    {
        std::wcout << "\n";
        std::wcout << L"               *             ,MMM8&&&.            *" << "\n";
        std::wcout << L"                            MMMM88&&&&&    ." << "\n";
        std::wcout << L"                           MMMM88&&&&&&&" << "\n";
        std::wcout << L"       *                   MMM88&&&&&&&&" << "\n";
        std::wcout << L"                           MMM88&&&&&&&&" << "\n";
        std::wcout << L"                           'MMM88&&&&&&'" << "\n";
        std::wcout << L"                             'MMM8&&&'      *" << "\n";
        std::wcout << L"            |\\___/|" << "\n";
        std::wcout << L"            )     (             .              " << "\n";
        std::wcout << L"           =\\     /=" << "\n";
        std::wcout << L"             )===(       *" << "\n";
        std::wcout << L"            /     \\" << "\n";
        std::wcout << L"            |     |" << "\n";
        std::wcout << L"           /       \\" << "\n";
        std::wcout << L"           \\       /" << "\n";
        std::wcout << L"    _/\\_/\\_/\\__  _/_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_" << "\n";
        std::wcout << L"    |  |  |  |( (  |  |  |  |  |  |  |  |  |  |" << "\n";
        std::wcout << L"    |  |  |  | ) ) |  |  |  |  |  |  |  |  |  |" << "\n";
        std::wcout << L"    |  |  |  |(_(  |  |  |  |  |  |  |  |  |  |" << "\n";
        std::wcout << L"    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |" << "\n";
        std::wcout << L"    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |" << "\n";
        std::wcout << "\n";
        std::wcout << L"Please donate to your local cat charity or shelter.\n\n";
        std::wcout << L"A big thanks to my cats: Rutherford, Freeman, and Maxwell" << "\n";
        std::wcout << "\n";
        std::wcout << L"        www.maximumoctopus.com/developmentcats.htm" << "\n";
        std::wcout << std::endl;
    }
}