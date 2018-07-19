#pragma once

#include "Kernel/PluginBase.h"

#include "Config/String.h"

namespace Mengine
{
    class AndroidNativeDevToDevPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeDevToDev" )

    public:
        AndroidNativeDevToDevPlugin();
        ~AndroidNativeDevToDevPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool initializeSDK( const String & _appId, const String & _secret, const String & _apiKey );
        bool onTutorialEvent( int _stateOrStep );
        bool setCurrentLevel( int _level );
        bool onLevelUp( int _level );
        bool onCurrencyAccrual( const String & _currencyName, int _currencyAmount, int _accrualType );
        bool onRealPayment( const String & _paymentId, float _inAppPrice, const String & _inAppName, const String & _inAppCurrencyISOCode );
        bool onInAppPurchase( const String & _purchaseId, const String & _purchaseType, int _purchaseAmount, int _purchasePrice, const String & _purchaseCurrency );
        bool onSimpleCustomEvent( const String & _eventName );
    };
}
