#pragma once

#include "AppleGameCenterInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

#import "AppleGameCenterNative.h"

namespace Mengine
{
	class AppleGameCenterService
		: public ServiceBase<AppleGameCenterInterface>
	{
	public:
		AppleGameCenterService();
		~AppleGameCenterService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
	protected:
        bool connect() override;

	protected:
        void reportAchievement( const ConstString & _achievementName, float _percentComplete ) override;
        bool checkAchievement( const ConstString & _achievementName ) const override;

	protected:
        void reportScore( const ConstString & _key, uint32_t _score ) override;

	protected:
		AppleGameCenterNative * m_gameCenterNative;

        bool m_achievementsLoaded;
		VectorConstString m_achievementsComplete;
    };
}
