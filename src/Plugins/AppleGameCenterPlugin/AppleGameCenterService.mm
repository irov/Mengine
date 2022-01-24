#include "AppleGameCenterService.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterService::AppleGameCenterService()
        : m_gameCenterNative( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterService::~AppleGameCenterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::_initializeService()
    {
        m_gameCenterNative = [[AppleGameCenterNative alloc]init];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterService::_finalizeService()
    {
        m_gameCenterNative = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterService::connect()
    {
        [m_gameCenterNative login : ^ (BOOL success)
        {
            NSLog( @"connect callback %s", success ? "True" : "FALSE" );

            [m_gameCenterNative loadCompletedAchievements : ^ (NSArray * completedAchievements)
            {
                NSLog( @"Completed Achievements %@", completedAchievements );
                for( NSString * ach in completedAchievements )
                {
                    const Char * ach_str = [ach UTF8String];
                    
                    m_achievementsComplete.push_back( Helper::stringizeString(ach_str) );
                }
            }] ;
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterService::reportAchievement( const ConstString & _achievementName, float _percentComplete )
    {
        NSString * nsDescription = [NSString stringWithUTF8String : _achievementName.c_str()];

        [m_gameCenterNative reportAchievementIdentifier : nsDescription percentComplete : _percentComplete withBanner : YES] ;

        if( _percentComplete >= 100.f )
        {
            m_achievementsComplete.push_back( _achievementName );
        }
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
    void AppleGameCenterService::reportScore( const ConstString & _key, uint32_t _score )
    {
        NSString* category = [NSString stringWithUTF8String:_key.c_str()];

        [m_gameCenterNative reportScore:_score forCategory:category];
    }
    //////////////////////////////////////////////////////////////////////////
}
