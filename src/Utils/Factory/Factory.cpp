#	include "Factory.h"
#	include "Factorable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factory::Factory()
		: m_count(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factory::~Factory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::setListener( const FactoryListenerInterfacePtr & _listener )
	{
		m_listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * Factory::createObject()
	{
		THREAD_GUARD_CHECK(this, nullptr, "Factory::createObject");

		++m_count;
		intrusive_ptr_add_ref( this );

		Factorable * object = this->_createObject();
		object->setFactory( this );

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::destroyObject( Factorable * _object )
	{	
		THREAD_GUARD_CHECK(this, nullptr, "Factory::destroyObject");

		if( m_listener != nullptr )
		{
			m_listener->onFactoryDestroyObject( _object );
		}

		this->_destroyObject( _object );	

		--m_count;
		intrusive_ptr_dec_ref( this );
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Factory::countObject() const
	{
		return m_count;
	}
}
