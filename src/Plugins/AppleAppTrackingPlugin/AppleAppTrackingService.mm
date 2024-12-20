#include "AppleAppTrackingService.h"

#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"

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
        , m_idfa( @"00000000-0000-0000-0000-000000000000" )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingService::~AppleAppTrackingService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::authorization( const LambdaAuthorizationResponse & _response )
    {
        if( @available(iOS 14.0, *) )
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
                
                EAppleAppTrackingAuthorization status_copy = m_status;
                NSString * idfa_copy = m_idfa;
                
                Helper::dispatchMainThreadEvent([copy_response, status_copy, idfa_copy]() {
                    const Char * idfa_str = [idfa_copy UTF8String];
                    
                    copy_response( status_copy, idfa_str );
                });
            }];
            
            return;
        }

        m_status = EAATA_AUTHORIZED;

        this->makeIDFA_();

        const Char * idfa_str = [m_idfa UTF8String];

        _response( m_status, idfa_str );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::getIDFA( EAppleAppTrackingAuthorization * const _status, Char * const _idfa ) const
    {
        *_status = m_status;
        
        const Char * idfa_str = [m_idfa UTF8String];
        
        StdString::strcpy( _idfa, idfa_str );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::makeIDFA_()
    {
        NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

        if( idfa_uuid == nil )
        {
            return;
        }

        NSString * idfa = [idfa_uuid UUIDString];
        
        m_idfa = idfa;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingService::isTrackingAllowed() const
    {
        if( @available(iOS 14.5, *) )
        {
            switch( ATTrackingManager.trackingAuthorizationStatus )
            {
                case ATTrackingManagerAuthorizationStatusAuthorized:
                {
                    return true;
                }break;
                case ATTrackingManagerAuthorizationStatusDenied:
                {
                    return false;
                }break;
                case ATTrackingManagerAuthorizationStatusNotDetermined:
                {
                    return false;
                }break;
                case ATTrackingManagerAuthorizationStatusRestricted:
                {
                    return false;
                }break;
            }
            
            return false;
        }

        if( [iOSDetail isValidIDFA] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
