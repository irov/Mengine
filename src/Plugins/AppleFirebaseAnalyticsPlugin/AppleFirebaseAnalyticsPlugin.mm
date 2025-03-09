#include "AppleFirebaseAnalyticsPlugin.h"

#import "AppleFirebaseAnalyticsEventProvider.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseAnalytics, Mengine::AppleFirebaseAnalyticsPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsPlugin::AppleFirebaseAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsPlugin::~AppleFirebaseAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseAnalyticsPlugin::_initializePlugin()
    {
        AnalyticsEventProviderInterfacePtr provider = Helper::makeFactorableUnique<AppleFirebaseAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );
        
        ANALYTICS_SERVICE()
            ->addEventProvider( provider );
        
        m_provider = provider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsPlugin::_finalizePlugin()
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
