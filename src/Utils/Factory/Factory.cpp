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
	void Factory::setMutex( const ThreadMutexInterfacePtr & _mutex )
	{
		m_mutex = _mutex;
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * Factory::createObject()
	{
		Factorable * object = nullptr;

		if( m_mutex == nullptr )
		{
			STDEX_THREAD_GUARD_CHECK( this, "Factory::createObject" );

			++m_count;
			intrusive_ptr_add_ref( this );

			object = this->_createObject();
			object->setFactory( this );
		}
		else
		{
			m_mutex->lock();

			++m_count;
			intrusive_ptr_add_ref( this );

			object = this->_createObject();
			object->setFactory( this );

			m_mutex->unlock();
		}

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::destroyObject( Factorable * _object )
	{	
		if( m_mutex == nullptr )
		{
			STDEX_THREAD_GUARD_CHECK( this, "Factory::destroyObject" );

			if( m_listener != nullptr )
			{
				m_listener->onFactoryDestroyObject( _object );
			}

			this->_destroyObject( _object );

			--m_count;
			intrusive_ptr_dec_ref( this );
		}
		else
		{
			m_mutex->lock();

			if( m_listener != nullptr )
			{
				m_listener->onFactoryDestroyObject( _object );
			}

			this->_destroyObject( _object );

			--m_count;
			intrusive_ptr_dec_ref( this );

			m_mutex->unlock();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Factory::countObject() const
	{
		return m_count;
	}
}
