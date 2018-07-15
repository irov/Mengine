#pragma once

#include "Core/PluginBase.h"

#include "Config/VectorString.h"

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
        bool onTutorialEvent( const int _stateOrStep );
        bool setCurrentLevel( const int _level );
        bool onLevelUp( const int _level );
        bool onCurrencyAccrual( const String & _currencyName, const int _currencyAmount, const int _accrualType );
        bool onRealPayment( const String & _paymentId, const float _inAppPrice, const String & _inAppName, const String & _inAppCurrencyISOCode );
        bool onInAppPurchase( const String & _purchaseId, const String & _purchaseType, const int _purchaseAmount, const int _purchasePrice, const String & _purchaseCurrency );
        bool onSimpleCustomEvent( const String & _eventName );
    };
}
