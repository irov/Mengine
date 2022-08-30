#pragma once

#include "AppleAppTrackingInterface.h"

#include "Kernel/ServiceBase.h"

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
        void getIDFA( EAppleAppTrackingAuthorization * const _status, ConstString * const _idfa ) const override;
        
    protected:
        void makeIDFA_();
        
    protected:
        EAppleAppTrackingAuthorization m_status;
        ConstString m_idfa;
    };
}
