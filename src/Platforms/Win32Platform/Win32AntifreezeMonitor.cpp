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
    AntifreezeData * g_antifreezeData = nullptr;
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::run()
    {
        g_antifreezeData = new AntifreezeData;

		g_antifreezeData->seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 5U );
        g_antifreezeData->refalive = 0;
        g_antifreezeData->run = true;

        g_antifreezeData->th = std::thread( []()
        {
            for( ; g_antifreezeData->run;)
            {
                uint64_t oldref = g_antifreezeData->refalive;
                std::this_thread::sleep_for( std::chrono::seconds( g_antifreezeData->seconds ) );

                if( oldref == g_antifreezeData->refalive && oldref != 0 )
                {
                    PLATFORM_SERVICE()
                        ->createProcessDump();

					LOGGER_ERROR( "Antifreeze monitor detect freeze process for %u seconds, and create dump!"
						, g_antifreezeData->seconds.load()
					);

                    break;
                }
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::stop()
    {
        if( g_antifreezeData != nullptr )
        {
            g_antifreezeData->run = false;
            g_antifreezeData->th.join();

            delete g_antifreezeData;
            g_antifreezeData = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        if( g_antifreezeData != nullptr )
        {
            g_antifreezeData->refalive++;
        }
    }
}
