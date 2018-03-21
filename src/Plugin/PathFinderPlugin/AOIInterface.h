#pragma once

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////	
	class AOIActorProviderInterface
	{
	};
	//////////////////////////////////////////////////////////////////////////
	class AOITriggerProviderInterface
	{
	public:
		virtual bool onAOIActorTest( AOIActorProviderInterface * _actor ) const = 0;

	public:
		virtual void onAOIActorEnter( AOIActorProviderInterface * _actor ) = 0;
		virtual void onAOIActorLeave( AOIActorProviderInterface * _actor ) = 0;
	};
}