#include "AppleDevToDevPlugin.h"

#include "Interface/AnalyticsServiceInterface.h"

#include "AppleDevToDevAnalyticsEventProvider.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleDevToDev, Mengine::AppleDevToDevPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::~AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevPlugin::_initializePlugin()
    {
        AnalyticsEventProviderInterfacePtr provider = Helper::makeFactorableUnique<AppleDevToDevAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );
        
        ANALYTICS_SERVICE()
            ->addEventProvider( provider );
        
        m_provider = provider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevPlugin::_finalizePlugin()
    {
        if( m_provider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_provider );
            
            m_provider = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
