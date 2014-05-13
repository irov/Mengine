#	include "Factory.h"
#	include "Factorable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factory::Factory()
		: m_listener(nullptr)
		, m_count(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factory::~Factory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * Factory::createObject()
	{
		THREAD_GUARD_CHECK(this);

		++m_count;
		intrusive_ptr_add_ref( this );

		Factorable * object = this->_createObject();
		object->setFactory( this );

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::destroyObject( Factorable * _object )
	{	
		THREAD_GUARD_CHECK(this);

		if( m_listener != nullptr )
		{
			m_listener->onFactoryDestroyObject( _object );
		}

		this->_destroyObject( _object );	

		--m_count;
		intrusive_ptr_dec_ref( this );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Factory::countObject() const
	{
		return m_count;
	}
}
