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


#include <iostream>
#include "Constants.h"
#include "Help.h"


namespace Help
{
	void ShowHelp()
	{
        std::cout << "\n" << "    QWait " << Constants::QWVersion << " :: " << Constants::QWDate << "\n";
        std::cout << "      (c) Paul Alan Freshney 2022" << "\n";
        std::cout << "\n";
        std::cout << "          https://qwait.sourceforge.io :: paul@freshney.org" << "\n";
        std::cout << "\n";
        std::cout << "  Use \"QWait /?\" for this help." << "\n";
        std::cout << "\n";
        std::cout << "    /Visitors;x                 set visitor count." << "\n";
        std::cout << "    /Template;x                 where is x is the template to use:" << "\n";
        std::cout << "                                    0 : test park" << "\n";
        std::cout << "                                    1 : Animal Kingom" << "\n";
        std::cout << "                                    2 : EPCOT" << "\n";
        std::cout << "                                    3 : Hollywood Studios" << "\n";
        std::cout << "                                    4 : Magic Kingdom" << "\n";
        std::cout << "                                    5 : Coin-op arcade" << "\n";
        std::cout << "    /UseConfigFile;filename     set configuration from specified config file." << "\n";
        std::cout << "\n";
        std::cout << "    /ImportVisitorDemo;filename rather than generating random visitors, the data from." << "\n";
        std::cout << "                                the specified file is used. Companion of /ReportVisitorDemo." << "\n";
        std::cout << "                                ignores the value set from /Visitors;x" << "\n";
        std::cout << "\n";
        std::cout << "    /DebugSCC                   export selection choice cache." << "\n";
        std::cout << "                                this is the data used by the visitor types to pick rides." << "\n";
        std::cout << "    /DebugNoExecute             everything except running the model." << "\n";
        std::cout << "\n";
        std::cout << "    /ReportHTML                 export HTML simulation summary report." << "\n";
        std::cout << "\n";
        std::cout << "    /ReportCVD                  export complete visitor data." << "\n";
        std::cout << "    /ReportMXM                  export a minute by minute report." << "\n";
        std::cout << "    /ReportSimulation           important information regarding the last sim." << "\n";
        std::cout << "    /ReportVisitorLocation      exports complete minute by minute report." << "\n";
        std::cout << "                                of visitor location. Warning! This generates" << "\n";
        std::cout << "                                very large output: ~280MB per 20000 visitors!" << "\n";
        std::cout << "    /ReportVisitorDemo;filename export visitor demographics (useful if you want" << "\n";
        std::cout << "                                to maintain a consistent set of visitors across" << "\n";
        std::cout << "                                multuple runs)." << "\n";
        std::cout << std::endl;
	}
}