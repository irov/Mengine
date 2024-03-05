#include "AppleMARSDKService.h"

#include "Environment/iOS/iOSDetail.h"
#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleBundle.h"
#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

#include "Config/Algorithm.h"

#import "MARSDKCore/MARSDKCore.h"
#import "EPSDK/EP_AppStorePay.h"

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
        
#ifdef MENGINE_DEBUG
        [[MARSDK sharedInstance] openXcodeLog];
#endif
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::_finalizeService()
    {
        [[MARSDK sharedInstance] setDelegate:nil];
        
        m_delegate = nil;
        m_adRewardedDelegate = nil;
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
        LOGGER_MESSAGE( "submitExtendedData data: %s"
            , _data
        );
        
        MARUserExtraData * extraData = [MARUserExtraData dataFromJsonString:@(_data)];
        
        [[MARSDK sharedInstance] submitExtraData:extraData];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitPaymentData( const Char * _data )
    {
        LOGGER_MESSAGE( "submitPaymentData data: %s"
            , _data
        );
        
        MARProductInfo * productInfo = [MARProductInfo productFromJsonString:@(_data)];
        
        if( productInfo == nil )
        {
            LOGGER_ERROR( "invalid create product from data: %s"
                , _data
            );
            
            return;
        }
        
        [[MARSDK sharedInstance] pay:productInfo];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::propComplete( const ConstString & _orderId )
    {
        LOGGER_MESSAGE( "propComplete orderId: %s"
            , _orderId.c_str()
        );
        
        AppleMARSDKProviderInterfacePtr copy_provider = m_provider;
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
                
                if( copy_provider == nullptr )
                {
                    LOGGER_ERROR( "not setup MARSDK provider" );
                    
                    return;
                }
                
                Helper::dispatchMainThreadEvent([copy_provider, copy_orderId]() {
                    copy_provider->onPropError( copy_orderId );
                });
                
                return;
            }

            LOGGER_MESSAGE( "prop complete '%s' response sucessful data: %s"
                , copy_orderId.c_str()
                , Helper::NSIdToString( _data ).c_str()
            );
            
            if( copy_provider == nullptr )
            {
                LOGGER_ERROR( "not setup MARSDK provider" );
                
                return;
            }
                
            Helper::dispatchMainThreadEvent([copy_provider, copy_orderId]() {
                copy_provider->onPropComplete( copy_orderId );
            });
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::requestNonConsumablePurchased()
    {
        LOGGER_MESSAGE( "requestNonConsumablePurchased");
        
        AppleMARSDKProviderInterfacePtr copy_provider = m_provider;
        
        [[EP_AppStorePay sharedInstance] setPurchasedNonConsumableArray:^(NSArray * _Nonnull purchasedNonConsumableArray) {
            LOGGER_MESSAGE( "onPurchasedNonConsumable: %s"
                , [[NSString stringWithFormat:@"%@", purchasedNonConsumableArray] UTF8String]
            );

            VectorConstString purchased;
            for( NSString * purchase in purchasedNonConsumableArray) {
                ConstString purchase_cstr = Helper::NSStringToConstString( purchase );
                
                purchased.emplace_back( purchase_cstr );
            }
            
            if( copy_provider == nullptr )
            {
                LOGGER_ERROR( "not setup MARSDK provider" );
                
                return;
            }
            
            Helper::dispatchMainThreadEvent([copy_provider, purchased]() {
                copy_provider->onPurchasedNonConsumable( purchased );
            });
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum )
    {
        LOGGER_MESSAGE( "showRewardVideoAd itemName: %s itemNum: %u"
            , _itemName.c_str()
            , _itemNum
        );
        
        UIViewController * rootViewController = Helper::iOSGetRootViewController();
        
        [[MARAd sharedInstance] showRewardVideoAd:rootViewController
                                         itemName:@(_itemName.c_str())
                                          itemNum:_itemNum
                                         delegate:m_adRewardedDelegate];
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AppleMARSDKService::getInternetDate() const
    {
        NSInteger date = [[MARSDK sharedInstance] getInternetDate];
        
        return date;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogin( const MapParams & _params )
    {
        LOGGER_MESSAGE( "onUserLogin" );
        
        if( m_provider == nullptr )
        {
            LOGGER_ERROR( "not setup MARSDK provider" );
            
            return;
        }
        
        AppleMARSDKProviderInterfacePtr copy_provider = m_provider;
        
        Helper::dispatchMainThreadEvent([copy_provider, _params]() {
            copy_provider->onUserLogin( _params );
        });
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogout( const MapParams & _params )
    {
        LOGGER_MESSAGE( "onUserLogout" );
        
        if( m_provider == nullptr )
        {
            LOGGER_ERROR( "not setup MARSDK provider" );
            
            return;
        }
        
        AppleMARSDKProviderInterfacePtr copy_provider = m_provider;
        
        Helper::dispatchMainThreadEvent([copy_provider, _params]() {
            copy_provider->onUserLogout( _params );
        });
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onPayPaid( const MapParams & _params )
    {
        LOGGER_MESSAGE( "onPayPaid" );
        
        if( m_provider == nullptr )
        {
            LOGGER_ERROR( "not setup MARSDK provider" );
            
            return;
        }
        
        AppleMARSDKProviderInterfacePtr copy_provider = m_provider;
        
        Helper::dispatchMainThreadEvent([copy_provider, _params]() {
            copy_provider->onPayPaid( _params );
        });
    }
    //////////////////////////////////////////////////////////////////////////
}
