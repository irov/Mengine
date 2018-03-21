#pragma once

#include "AOIInterface.h"

#include "Factory/FactorablePtr.h"

#   include "stdex/intrusive_ptr.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class AOIActor
		: public FactorablePtr
	{
	public:
		AOIActor();
		~AOIActor();

	public:
		void setProvider( AOIActorProviderInterface * _provider );
		AOIActorProviderInterface * getProvider();

	public:
		void remove();
		bool isRemoved() const;

	protected:
		AOIActorProviderInterface * m_provider;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<AOIActor> AOIActorPtr;
}