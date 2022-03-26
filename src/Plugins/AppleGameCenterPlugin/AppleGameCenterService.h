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
        
    public:
        void setProvider( const AppleGameCenterProviderInterfacePtr & _provider ) override;
        const AppleGameCenterProviderInterfacePtr & getProvider() const override;
        
	protected:
        bool connect() override;
        bool isConnect() const override;

	protected:
        bool reportAchievement( const ConstString & _achievementName, float _percentComplete, const LambdaAchievemtResponse & _response ) override;
        bool checkAchievement( const ConstString & _achievementName ) const override;

	protected:
        bool reportScore( const ConstString & _key, int64_t _score, const LambdaScoreResponse & _response ) override;

	protected:
        AppleGameCenterProviderInterfacePtr m_provider;
        
		AppleGameCenterNative * m_gameCenterNative;

		VectorConstString m_achievementsComplete;
        bool m_gameCenterAuthenticate;
        bool m_achievementsSynchronization;
    };
}
