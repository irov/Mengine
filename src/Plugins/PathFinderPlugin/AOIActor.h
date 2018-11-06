#pragma once

#include "AOIInterface.h"

#include "Factory/Factorable.h"
#include "Core/IntrusivePtr.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class AOIActor
		: public Factorable
	{
	public:
		AOIActor();
		~AOIActor() override;

	public:
		void setProvider( const AOIActorProviderInterfacePtr & _provider );
		const AOIActorProviderInterfacePtr & getProvider();

	public:
		void remove();
		bool isRemoved() const;

	protected:
		AOIActorProviderInterfacePtr m_provider;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AOIActor> AOIActorPtr;
}