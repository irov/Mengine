#include "AppleGameCenterService.h"

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
            NSLog( @"connect callback %s", success ? "True" : "FALSE" );
            
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

            [m_gameCenterNative loadCompletedAchievements : ^ (NSError * _Nullable error, NSArray * _Nullable completedAchievements) {
                if (error) {
                    return;
                }
                
                if (!completedAchievements) {
                    return;
                }
                
                NSLog( @"Completed Achievements %@", completedAchievements );
                
                for( NSString * ach in completedAchievements )
                {
                    const Char * ach_str = [ach UTF8String];
                    
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
        NSString * nsDescription = [NSString stringWithUTF8String : _achievementName.c_str()];

        BOOL result = [m_gameCenterNative reportAchievementIdentifier:nsDescription percentComplete:_percentComplete withBanner:YES response:^(NSError * _Nullable _error) {
            if (_error) {
                _response(false, 0.f);
                return;
            }
            
            if( _percentComplete >= 100.f )
            {
                m_achievementsComplete.push_back( _achievementName );
            }
            
            _response(true, _percentComplete);
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
    bool AppleGameCenterService::reportScore( const ConstString & _key, uint32_t _score, const LambdaScoreResponse & _response )
    {
        NSString* category = [NSString stringWithUTF8String:_key.c_str()];

        BOOL result = [m_gameCenterNative reportScore:_score forCategory:category response:^(NSError * _Nullable _error) {
            if (_error) {
                _response(false);
                
                return;
            }
            
            _response(true);
        }];
        
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
