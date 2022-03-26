#include "AppleGameCenterService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleGameCenterService, Mengine::AppleGameCenterService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterService::AppleGameCenterService()
        : m_gameCenterNative( nil )
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
        m_gameCenterNative = [[AppleGameCenterNative alloc] init];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterService::_finalizeService()
    {
        m_provider = nullptr;
        
        [m_gameCenterNative release];
        m_gameCenterNative = nil;
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
        [m_gameCenterNative login : ^ (BOOL success)
        {
            LOGGER_MESSAGE( "[AppleGameCenter] connect: '%s'"
                , success == TRUE ? "Successful" : "Failed"
            );
            
            if(!success) {
                m_gameCenterAuthenticate = false;
                m_achievementsSynchronization = false;
                m_achievementsComplete.clear();
                
                if( m_provider != nullptr )
                {
                    m_provider->onAppleGameConterAuthenticate( false );
                }
                
                return;
            }
            
            if(m_achievementsSynchronization) {
                return;
            }

            [m_gameCenterNative loadCompletedAchievements : ^ (NSError * _Nullable _error, NSArray * _Nullable _completedAchievements) {
                if (_error) {
                    LOGGER_ERROR("[AppleGameCenter] load completed achievements error: '%s'"
                       , Helper::AppleGetMessageFromNSError(_error).c_str()
                    );
                    
                    return;
                }
                
                if (!_completedAchievements) {
                    return;
                }
                
                for( NSString * ach in _completedAchievements )
                {
                    const Char * ach_str = [ach UTF8String];
                    
                    LOGGER_MESSAGE( "[AppleGameCenter] completed achievement: '%s'"
                        , ach_str
                    );
                    
                    m_achievementsComplete.push_back( Helper::stringizeString(ach_str) );
                }
                
                m_achievementsSynchronization = true;
                
                if( m_provider != nullptr )
                {
                    m_provider->onAppleGameConterAuthenticate( true );
                }
                
                m_gameCenterAuthenticate = true;
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
    bool AppleGameCenterService::reportAchievement( const ConstString & _achievementName, float _percentComplete, const LambdaAchievemtResponse & _response )
    {
        LOGGER_MESSAGE( "[AppleGameCenter] report achievement: '%s' [%f]"
            , _achievementName.c_str()
            , _percentComplete
        );
        
        NSString * nsDescription = [NSString stringWithUTF8String : _achievementName.c_str()];
        

        ConstString copy_achievementName = _achievementName;
        LambdaAchievemtResponse copy_response = _response;

        BOOL result = [m_gameCenterNative reportAchievementIdentifier:nsDescription percentComplete:_percentComplete withBanner:YES response:^(NSError * _Nullable _error) {
            if (_error) {
                LOGGER_ERROR("[AppleGameCenter] response achievement '%s' [%f] error: %s"
                   , copy_achievementName.c_str()
                   , _percentComplete
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                _response( false, 0.f );
                
                return;
            }
            
            LOGGER_MESSAGE( "[AppleGameCenter] response achievement '%s' [%f] successful"
                , copy_achievementName.c_str()
                , _percentComplete
            );
            
            if( _percentComplete >= 100.f )
            {
                m_achievementsComplete.push_back( copy_achievementName );
            }
            
            copy_response(true, _percentComplete);
        }];
        
        return result;
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
    bool AppleGameCenterService::reportScore( const ConstString & _key, int64_t _score, const LambdaScoreResponse & _response )
    {
        LOGGER_MESSAGE( "[AppleGameCenter] report score: '%s' [%lld]"
            , _key.c_str()
            , _score
        );
        
        NSString* identifier = [NSString stringWithUTF8String:_key.c_str()];
        
        ConstString copy_key = _key;
        LambdaScoreResponse copy_response = _response;

        BOOL result = [m_gameCenterNative reportScore:identifier score:_score response:^(NSError * _Nullable _error) {
            if (_error) {
                LOGGER_ERROR("[AppleGameCenter] response score '%s' [%lld] error: %s"
                   , copy_key.c_str()
                   , _score
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                copy_response( false );
                
                return;
            }
            
            LOGGER_MESSAGE( "[AppleGameCenter] response score '%s' [%lld] successful"
                , copy_key.c_str()
                , _score
            );
            
            copy_response( true );
        }];
        
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
