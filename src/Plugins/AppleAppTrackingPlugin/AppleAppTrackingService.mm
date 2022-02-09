#include "AppleAppTrackingService.h"

#include "Kernel/Logger.h"

#import "AppleAppTrackingPlugin-Swift.h"

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
        //Empty

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
        LambdaAuthorizationResponse copy_response = _response;
        
        [AppleAppTrackingNative requestAuthorizationWithCb:^(NSInteger _code, NSString * _Nullable _idfa) {
            uint32_t code_u32 = (uint32_t)_code;
            
            switch( code_u32 )
            {
            case 0:
                {
                    m_status = EAATA_AUTHORIZED;
                    
                    const char * idfa_str = [_idfa UTF8String];
                    m_idfa = Helper::stringizeString( idfa_str );
                }break;
            case 1:
                {
                    m_status = EAATA_DENIED;
                }break;
            case 2:
                {
                    m_status = EAATA_RESTRICTED;
                }break;
            case 3:
                {
                    m_status = EAATA_NOT_DETERMINED;
                }break;
            default:
                {
                    m_status = EAATA_NONE;
                    
                    LOGGER_ERROR("invalid AppTracking result code [%u]"
                        , code_u32
                    );
                }break;
            }
            
            copy_response( m_status, m_idfa );
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingService::getIDFA( EAppleAppTrackingAuthorization * const _status, ConstString * const _idfa ) const
    {
        *_status = m_status;
        *_idfa = m_idfa;
    }
    //////////////////////////////////////////////////////////////////////////
}
