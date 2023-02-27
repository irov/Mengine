#pragma once

#include "AppleMARSDKInterface.h"

#include "AppleMARSDKDelegate.h"
#include "AppleMARSDKAdRewardedDelegate.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
	class AppleMARSDKService
		: public ServiceBase<AppleMARSDKServiceInterface>
	{
	public:
        AppleMARSDKService();
		~AppleMARSDKService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void setProvider( const AppleMARSDKProviderInterfacePtr & _provider ) override;
        const AppleMARSDKProviderInterfacePtr & getProvider() const override;
        
    public:
        bool login() override;
        bool logout() override;
        bool switchAccount() override;
        
        void submitExtendedData( const Char * _data ) override;
        void submitPaymentData( const Char * _data ) override;
        
        void propComplete( const ConstString & _productId ) override;
        
    public:
        void showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum ) override;
        
    public:
        void onUserLogin( const MARSDKResultParams & _params );
        void onUserLogout( const MARSDKResultParams & _params );
        void onPayPaid( const MARSDKResultParams & _params );

	protected:
        AppleMARSDKProviderInterfacePtr m_provider;
        
        AppleMARSDKDelegate * m_delegate;
        AppleMARSDKAdRewardedDelegate * m_adRewardedDelegate;
    };
}
