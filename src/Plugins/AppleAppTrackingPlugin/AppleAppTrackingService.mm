#include "AppleAppTrackingService.h"

#include "Kernel/Logger.h"

#import <AppTrackingTransparency/AppTrackingTransparency.h>
#import <AdSupport/AdSupport.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAppTrackingService, Mengine::AppleAppTrackingService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingService::AppleAppTrackingService()
        : m_status( EAATA_NONE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingService::~AppleAppTrackingService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingService::_initializeService()
    {
        m_idfa = STRINGIZE_STRING_LOCAL("00000000-0000-0000-0000-000000000000");

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::_finalizeService()
    {
        m_idfa.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::authorization( const LambdaAuthorizationResponse & _response )
    {
        if(@available(iOS 14.0, *))
        {
            LambdaAuthorizationResponse copy_response = _response;
            
            [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
                switch( status )
                {
                    case ATTrackingManagerAuthorizationStatusAuthorized:
                    {
                        m_status = EAATA_AUTHORIZED;
                        
                        this->makeIDFA_();
                    }break;
                    case ATTrackingManagerAuthorizationStatusDenied:
                    {
                        m_status = EAATA_DENIED;
                    }break;
                    case ATTrackingManagerAuthorizationStatusNotDetermined:
                    {
                        m_status = EAATA_NOT_DETERMINED;
                    }break;
                    case ATTrackingManagerAuthorizationStatusRestricted:
                    {
                        m_status = EAATA_RESTRICTED;
                    }break;
                }
                
                copy_response( m_status, m_idfa );
            }];
        }
        else
        {
            m_status = EAATA_AUTHORIZED;
                
            this->makeIDFA_();
                
            _response( m_status, m_idfa );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::getIDFA( EAppleAppTrackingAuthorization * const _status, ConstString * const _idfa ) const
    {
        *_status = m_status;
        MENGINE_STRCPY( _idfa, m_idfa );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::makeIDFA_()
    {
        Helper::iOSGetIDFA( m_idfa );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingService::isTrackingAllowed() const
    {
        if(@available(iOS 14.5, *))
        {
            if( ATTrackingManager.trackingAuthorizationStatus == ATTrackingManagerAuthorizationStatusDenied )
            {
                return false;
            }
        }
        else
        {
            if( Helper::iOSIsValidIDFA() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
