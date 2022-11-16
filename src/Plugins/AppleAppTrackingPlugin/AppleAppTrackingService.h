#pragma once

#include "AppleAppTrackingInterface.h"

#include "Kernel/ServiceBase.h"

#include "Const/Char.h"

namespace Mengine
{
	class AppleAppTrackingService
		: public ServiceBase<AppleAppTrackingInterface>
	{
	public:
        AppleAppTrackingService();
		~AppleAppTrackingService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
	protected:
		void authorization( const LambdaAuthorizationResponse & _response ) override;
        
    protected:
        void getIDFA( EAppleAppTrackingAuthorization * const _status, Char * const _idfa ) const override;

    protected:
        bool isTrackingAllowed() const override;
        
    protected:
        void makeIDFA_();
        
    protected:
        EAppleAppTrackingAuthorization m_status;

        Char m_idfa[64] = {'\0'};
    };
}
