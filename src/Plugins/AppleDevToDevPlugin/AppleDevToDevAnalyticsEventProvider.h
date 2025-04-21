#pragma once

#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
	class AppleDevToDevAnalyticsEventProvider
		: public AnalyticsEventProviderInterface
        , public Factorable
	{
	public:
        AppleDevToDevAnalyticsEventProvider();
		~AppleDevToDevAnalyticsEventProvider() override;
        
    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        
    protected:
		void onAnalyticsFlush() override;
    };
}
