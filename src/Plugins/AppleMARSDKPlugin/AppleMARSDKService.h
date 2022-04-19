#pragma once

#include "AppleMARSDKInterface.h"

#include "AppleMARSDKDelegate.h"

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
        
        void submitExtendedData( const MARSDKExtraData & extradata ) override;
        void submitPaymentData( const MARSDKProductInfo & product ) override;
        
    public:
        void onUserLogin();
        void onUserLogout();
        void onPayPaid();

	protected:
        AppleMARSDKProviderInterfacePtr m_provider;
        
        AppleMARSDKDelegate * m_delegate;
    };
}
