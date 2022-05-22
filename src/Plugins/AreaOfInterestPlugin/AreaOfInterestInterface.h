#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/Interface.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////	
	class AreaOfInterestActorProviderInterface
        : public Mixin
	{
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestActorProviderInterface> AreaOfInterestActorProviderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterestTriggerProviderInterface
        : public Mixin
	{
	public:
		virtual bool onAreaOfInterestActorTest( const AreaOfInterestActorProviderInterfacePtr & _actor ) const = 0;

	public:
		virtual void onAreaOfInterestActorEnter( const AreaOfInterestActorProviderInterfacePtr & _actor ) = 0;
		virtual void onAreaOfInterestActorLeave( const AreaOfInterestActorProviderInterfacePtr & _actor ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestTriggerProviderInterface> AreaOfInterestTriggerProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class AreaOfInterestActorInterface
        : public ServantInterface
	{
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AreaOfInterestActorInterface> AreaOfInterestActorInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterestTriggerInterface
		: public ServantInterface
	{
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AreaOfInterestTriggerInterface> AreaOfInterestTriggerInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterestZoneInterface
		: public ServantInterface
	{
    public:
        virtual AreaOfInterestTriggerInterfacePtr createTrigger( const AreaOfInterestTriggerProviderInterfacePtr & _provider, const DocumentPtr & _doc ) = 0;
        virtual void removeTriger( const AreaOfInterestTriggerInterfacePtr & _trigger ) = 0;

        virtual AreaOfInterestActorInterfacePtr createActor( const AreaOfInterestActorProviderInterfacePtr & _provider, const DocumentPtr & _doc ) = 0;
        virtual void removeActor( const AreaOfInterestActorInterfacePtr & _actor ) = 0;

    public:
        virtual void freeze( bool _value ) = 0;
        virtual bool isFreeze() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AreaOfInterestZoneInterface> AreaOfInterestZoneInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterestServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE( "AreaOfInterest" )

	public:
		virtual AreaOfInterestZoneInterfacePtr createZone( const DocumentPtr & _doc ) = 0;
        virtual void removeZone( const AreaOfInterestZoneInterfacePtr & _zone ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define AREAOFINTEREST_SERVICE()\
    ((Mengine::AreaOfInterestServiceInterface *)SERVICE_GET(Mengine::AreaOfInterestServiceInterface))
//////////////////////////////////////////////////////////////////////////