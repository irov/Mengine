#include "AppleMARSDKService.h"

#include "Environment/iOS/iOSDetail.h"
#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleDetail.h"
#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import "MARSDKCore/MARSDKCore.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleMARSDKService, Mengine::AppleMARSDKService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKService::AppleMARSDKService()
        : m_delegate( nil )
        , m_adRewardedDelegate( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKService::~AppleMARSDKService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::_initializeService()
    {
        m_delegate = [[AppleMARSDKDelegate alloc] initWithService:this];
        
        [[MARSDK sharedInstance] setDelegate:m_delegate];
        
        m_adRewardedDelegate = [[AppleMARSDKAdRewardedDelegate alloc] initWithService:this];
        
        NSDictionary * MARSDKConfig = Mengine::Helper::AppleGetBundlePluginConfig(@"MARSDK");
        [[MARSDK sharedInstance] initWithParams:MARSDKConfig];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::_finalizeService()
    {
        [[MARSDK sharedInstance] setDelegate:nil];
        
        if( m_delegate != nil )
        {
            [m_delegate release];
            m_delegate = nil;
        }
        
        if( m_adRewardedDelegate != nil )
        {
            [m_adRewardedDelegate release];
            m_adRewardedDelegate = nil;
        }
    }
    //////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::setProvider( const AppleMARSDKProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////
    const AppleMARSDKProviderInterfacePtr & AppleMARSDKService::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::login()
    {
        LOGGER_MESSAGE( "login" );
        
        [[MARSDK sharedInstance] login];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::logout()
    {
        LOGGER_MESSAGE( "logout" );
        
        [[MARSDK sharedInstance] logout];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::switchAccount()
    {
        LOGGER_MESSAGE( "switchAccount" );
        
        [[MARSDK sharedInstance] switchAccount];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitExtendedData( const Char * _data )
    {
        MARUserExtraData * extraData = [MARUserExtraData dataFromJsonString:@(_data)];
        
        LOGGER_MESSAGE( "submit extended data: %s"
            , _data
        );
        
        [[MARSDK sharedInstance] submitExtraData:extraData];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitPaymentData( const Char * _data )
    {
        MARProductInfo * productInfo = [MARProductInfo productFromJsonString:@(_data)];
        
        if( productInfo == nil )
        {
            LOGGER_ERROR( "invalid create product from data: %s"
                , _data
            );
            
            return;
        }

        LOGGER_MESSAGE( "submit payment data: %s"
            , _data
        );
        
        [[MARSDK sharedInstance] pay:productInfo];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::propComplete( const ConstString & _orderId )
    {
        LOGGER_MESSAGE( "prop complete orderId: %s"
            , _orderId.c_str()
        );
        
        ConstString copy_orderId = _orderId;
        
        [[MARSDK sharedInstance] propComplete:Helper::stringToNSString(_orderId)
                              responseHandler:^(NSURLResponse* _response, id _data, NSError * _connectionError) {
            if( _connectionError != nil )
            {
                LOGGER_ERROR( "prop complete '%s' response failed data: %s error: %s"
                    , copy_orderId.c_str()
                    , Helper::NSIdToString( _data ).c_str()
                    , Helper::AppleGetMessageFromNSError( _connectionError ).c_str()
                );
                
                if( m_provider == nullptr )
                {
                    return;
                }
                
                m_provider->onPropError( copy_orderId );
                
                return;
            }

            LOGGER_MESSAGE( "prop complete '%s' response sucessful data: %s"
                , copy_orderId.c_str()
                , Helper::NSIdToString( _data ).c_str()
            );
            
            if( m_provider == nullptr )
            {
                return;
            }
                
            m_provider->onPropComplete( copy_orderId );
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum )
    {
        LOGGER_MESSAGE( "show reward video ad: item '%s' num [%u]"
            , _itemName.c_str()
            , _itemNum
        );
        
        UIViewController * rootViewController = Helper::iOSGetRootViewController();
        
        [[MARAd sharedInstance] showRewardVideoAd:rootViewController
                                         itemName:[[NSString alloc] initWithUTF8String:_itemName.c_str()]
                                          itemNum:_itemNum
                                         delegate:m_adRewardedDelegate];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogin( const MARSDKResultParams & _params )
    {
        LOGGER_MESSAGE( "event user login" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogin( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogout( const MARSDKResultParams & _params )
    {
        LOGGER_MESSAGE( "event user logout" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogout( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onPayPaid( const MARSDKResultParams & _params )
    {
        LOGGER_MESSAGE( "event pay paid" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onPayPaid( _params );
    }
    //////////////////////////////////////////////////////////////////////////
}
