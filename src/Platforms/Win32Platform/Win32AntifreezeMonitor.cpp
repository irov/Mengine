#include "Win32AntifreezeMonitor.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"

#include <thread>
#include <atomic>
#include <chrono>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct AntifreezeData
    {
		std::atomic<uint32_t> seconds;
        std::atomic<uint32_t> refalive;
        std::atomic<bool> run;

		std::thread th;
    };
    //////////////////////////////////////////////////////////////////////////
    AntifreezeData g_antifreezeData;
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::run()
    {
        uint32_t seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 5U );

        if( seconds == 0U )
        {
            return;
        }

        g_antifreezeData.seconds = seconds;
        g_antifreezeData.refalive = 0;
        g_antifreezeData.run = true;

        g_antifreezeData.th = std::thread( []()
        {
            std::chrono::seconds pause_time( g_antifreezeData.seconds );

            for( ; g_antifreezeData.run;)
            {
                uint32_t oldref = g_antifreezeData.refalive;

                std::this_thread::sleep_for( pause_time );

                if( oldref == g_antifreezeData.refalive && oldref != 0 )
                {
                    PLATFORM_SERVICE()
                        ->createProcessDump();

					LOGGER_ERROR( "Antifreeze monitor detect freeze process for %u seconds, and create dump!"
						, g_antifreezeData.seconds.load()
					);

                    break;
                }
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::stop()
    {        
        if( g_antifreezeData.th.joinable() == true )
        {
            g_antifreezeData.run = false;
            g_antifreezeData.th.detach();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        g_antifreezeData.refalive++;        
    }
}
