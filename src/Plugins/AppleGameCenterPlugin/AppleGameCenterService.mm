#include "AppleGameCenterService.h"

#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleGameCenterService, Mengine::AppleGameCenterService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterService::AppleGameCenterService()
        : m_gameCenterDelegate( nil )
        , m_gameCenterAuthenticate( false )
        , m_achievementsSynchronization( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterService::~AppleGameCenterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::_initializeService()
    {
        m_gameCenterDelegate = [[AppleGameCenterDelegate alloc] init];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterService::_finalizeService()
    {
        m_provider = nullptr;
        
        m_gameCenterDelegate = nil;
    }
//////////////////////////////////////////////////////////////////////////
    void AppleGameCenterService::setProvider( const AppleGameCenterProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleGameCenterProviderInterfacePtr & AppleGameCenterService::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::connect()
    {
        [m_gameCenterDelegate login:^(NSError * _Nullable _error) {
            AppleGameCenterProviderInterfacePtr copy_provider = m_provider;
            
            if( _error != nil )
            {
                m_gameCenterAuthenticate = false;
                
                m_achievementsComplete.clear();
                
                LOGGER_ERROR( "login error: '%s'"
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                if( copy_provider != nullptr )
                {
                    Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                        copy_provider->onAppleGameCenterAuthenticate( false );
                    });
                }
                
                return;
            }
            
            LOGGER_MESSAGE( "connect successful" );
            
            if( m_gameCenterAuthenticate == false )
            {
                m_gameCenterAuthenticate = true;
                
                if( copy_provider != nullptr )
                {
                    Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                        copy_provider->onAppleGameCenterAuthenticate( true );
                    });
                }
            }
            
            m_achievementsSynchronization = false;
            m_achievementsComplete.clear();
            
            if( copy_provider != nullptr )
            {
                Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                    copy_provider->onAppleGameCenterSynchronizate( false );
                });
            }
            
            [m_gameCenterDelegate loadCompletedAchievements:^(NSError * _Nullable _error, NSArray * _Nullable _completedAchievements) {
                AppleGameCenterProviderInterfacePtr provider = m_provider;
                
                if( _error != nil )
                {
                    m_achievementsSynchronization = false;
                    
                    LOGGER_ERROR("load completed achievements error: '%s'"
                       , Helper::AppleGetMessageFromNSError(_error).c_str()
                    );
                    
                    if( provider != nullptr )
                    {
                        Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                            copy_provider->onAppleGameCenterSynchronizate( false );
                        });
                    }
                    
                    return;
                }
                
                if( _completedAchievements != nil )
                {
                    for( NSString * ach in _completedAchievements )
                    {
                        const Char * ach_str = [ach UTF8String];
                        
                        LOGGER_MESSAGE( "completed achievement: '%s'"
                            , ach_str
                        );
                        
                        m_achievementsComplete.push_back( Helper::stringizeString(ach_str) );
                    }
                }
                
                m_achievementsSynchronization = true;
                
                if( provider != nullptr )
                {
                    Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                        copy_provider->onAppleGameCenterSynchronizate( true );
                    });
                }
            }] ;
        }];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::isConnect() const
    {
        return m_gameCenterAuthenticate;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::isSynchronizate() const
    {
        return m_achievementsSynchronization;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::reportAchievement( const ConstString & _achievementName, double _percentComplete, const LambdaAchievemtResponse & _response )
    {
        LOGGER_MESSAGE( "report achievement: '%s' [%lf]"
            , _achievementName.c_str()
            , _percentComplete
        );
        
        NSString * nsDescription = [NSString stringWithUTF8String : _achievementName.c_str()];

        ConstString copy_achievementName = _achievementName;
        LambdaAchievemtResponse copy_response = _response;

        BOOL result = [m_gameCenterDelegate reportAchievementIdentifier:nsDescription percentComplete:_percentComplete withBanner:YES response:^(NSError * _Nullable _error) {
            if( _error != nil )
            {
                LOGGER_ERROR( "response achievement '%s' [%lf] error: %s"
                   , copy_achievementName.c_str()
                   , _percentComplete
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_response]() {
                    copy_response( false );
                });
                
                return;
            }
            
            LOGGER_MESSAGE( "response achievement '%s' [%lf] successful"
                , copy_achievementName.c_str()
                , _percentComplete
            );
            
            if( _percentComplete >= 100.0 )
            {
                m_achievementsComplete.push_back( copy_achievementName );
            }
            
            Mengine::Helper::dispatchMainThreadEvent([copy_response]() {
                copy_response( true );
            });
        }];
        
        if( result == FALSE )
        {
            LOGGER_ERROR( "invalid report achievement '%s' [%lf]"
               , _achievementName.c_str()
               , _percentComplete
            );
            
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::checkAchievement( const ConstString & _achievementName ) const
    {
        if( Algorithm::find( m_achievementsComplete.begin(), m_achievementsComplete.end(), _achievementName) == m_achievementsComplete.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::resetAchievements()
    {
        LOGGER_MESSAGE( "try reset achievemnts" );
        
        BOOL result = [m_gameCenterDelegate resetAchievements : ^ (NSError * _Nullable _error) {
            if( _error != nil )
            {
                LOGGER_ERROR( "reset achievemnts error: '%s'"
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                return;
            }
            
            LOGGER_MESSAGE( "reset achievement successful" );
        }] ;
        
        if( result == FALSE )
        {
            LOGGER_ERROR( "invalid reset achievements" );
            
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::reportScore( const ConstString & _key, int64_t _score, const LambdaScoreResponse & _response )
    {
        LOGGER_MESSAGE( "report score: '%s' [%lld]"
            , _key.c_str()
            , _score
        );
        
        NSString* identifier = [NSString stringWithUTF8String:_key.c_str()];
        
        ConstString copy_key = _key;
        LambdaScoreResponse copy_response = _response;

        BOOL result = [m_gameCenterDelegate reportScore:identifier score:_score response:^(NSError * _Nullable _error) {
            if( _error != nil )
            {
                LOGGER_ERROR( "response score '%s' [%lld] error: %s"
                   , copy_key.c_str()
                   , _score
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_response]() {
                    copy_response( false );
                });
                
                return;
            }
            
            LOGGER_MESSAGE( "response score '%s' [%lld] successful"
                , copy_key.c_str()
                , _score
            );
            
            Mengine::Helper::dispatchMainThreadEvent([copy_response]() {
                copy_response( true );
            });
        }];
        
        if( result == FALSE )
        {
            LOGGER_ERROR( "invalid report score '%s' [%lld]"
               , _key.c_str()
               , _score
            );
            
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
