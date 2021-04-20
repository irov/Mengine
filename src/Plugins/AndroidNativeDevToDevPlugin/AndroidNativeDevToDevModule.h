#pragma once

#include "DevToDevEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Kernel/String.h"
#include "Config/Lambda.h"

namespace Mengine
{
    class AndroidNativeDevToDevModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeDevToDevModule );

    public:
        typedef AndroidEventation<DevToDevEventHandler> DevToDevEventation;
        typedef typename DevToDevEventation::LambdaEventHandler LambdaDevToDevEventHandler;

    public:
        AndroidNativeDevToDevModule();
        ~AndroidNativeDevToDevModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaDevToDevEventHandler & _command );

    public:
        void setEventHandler( const DevToDevEventHandlerPtr & _handler );

    public:
        bool initializeSDK( const String & _appId, const String & _secret, const String & _apiKey );

    public:
        bool onTutorialEvent( int _stateOrStep );
        bool setCurrentLevel( int _level );
        bool onLevelUp( int _level );
        bool onCurrencyAccrual( const String & _currencyName, int _currencyAmount, int _accrualType );
        bool onRealPayment( const String & _paymentId, float _inAppPrice, const String & _inAppName, const String & _inAppCurrencyISOCode );
        bool onInAppPurchase( const String & _purchaseId, const String & _purchaseType, int _purchaseAmount, int _purchasePrice, const String & _purchaseCurrency );
        bool onSimpleCustomEvent( const String & _eventName, const String & _intJSON, const String & _floatJSON, const String & _stringJSON );

    protected:
        DevToDevEventation m_eventation;
    };
}
