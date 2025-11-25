#include "AppleAmplitudePlugin.h"

#include "Interface/AnalyticsServiceInterface.h"

#include "AppleAmplitudeAnalyticsEventProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAmplitude, Mengine::AppleAmplitudePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAmplitudePlugin::AppleAmplitudePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAmplitudePlugin::~AppleAmplitudePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAmplitudePlugin::_initializePlugin()
    {
        AnalyticsEventProviderInterfacePtr provider = Helper::makeFactorableUnique<AppleAmplitudeAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( provider );

        m_provider = provider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAmplitudePlugin::_finalizePlugin()
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

