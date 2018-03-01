#	pragma once

#	include "Factory.h"

#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class FactoryWithMutex
		: public Factory
	{
	public:
        FactoryWithMutex(const char * _name);
		virtual ~FactoryWithMutex();

	public:
		void setMutex( const ThreadMutexInterfacePtr & _mutex );
		
	public:
        PointerFactorable createObject() override;
		void destroyObject( Factorable * _object ) override;

	protected:		
		ThreadMutexInterfacePtr m_mutex;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<FactoryWithMutex> FactoryWithMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
