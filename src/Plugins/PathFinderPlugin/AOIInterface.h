#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////	
	class AOIActorProviderInterface
        : public Mixin
	{
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AOIActorProviderInterface> AOIActorProviderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class AOITriggerProviderInterface
        : public Mixin
	{
	public:
		virtual bool onAOIActorTest( const AOIActorProviderInterfacePtr & _actor ) const = 0;

	public:
		virtual void onAOIActorEnter( const AOIActorProviderInterfacePtr & _actor ) = 0;
		virtual void onAOIActorLeave( const AOIActorProviderInterfacePtr & _actor ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AOITriggerProviderInterface> AOITriggerProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}