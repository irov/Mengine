#pragma once

#include "AreaOfInterestInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterestActor
		: public AreaOfInterestActorInterface
	{
		DECLARE_FACTORABLE( AreaOfInterestActor );

	public:
		AreaOfInterestActor();
		~AreaOfInterestActor() override;

	public:
		void setProvider( const AreaOfInterestActorProviderInterfacePtr & _provider );
		const AreaOfInterestActorProviderInterfacePtr & getProvider() const;

	protected:
		AreaOfInterestActorProviderInterfacePtr m_provider;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AreaOfInterestActor, AreaOfInterestActorInterface> AreaOfInterestActorPtr;
	//////////////////////////////////////////////////////////////////////////
}