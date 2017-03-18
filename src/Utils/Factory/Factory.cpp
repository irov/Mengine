#	include "Factory.h"
#	include "Factorable.h"

#	include "Interface/FactoryInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factory::Factory(ServiceProviderInterface * _serviceProvider, const char * _name )
		: m_serviceProvider(_serviceProvider)
		, m_name(_name)
		, m_count(0)
	{
		FACTORY_SERVICE(m_serviceProvider)
            ->registerFactory( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Factory::~Factory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::setDestroyListener( const FactoryDestroyListenerInterfacePtr & _listener )
	{
		m_destroyListener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::setMutex( const ThreadMutexInterfacePtr & _mutex )
	{
		m_mutex = _mutex;
	}
	//////////////////////////////////////////////////////////////////////////
    PointerFactorable Factory::createObject()
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

			if( m_destroyListener != nullptr )
			{
				m_destroyListener->onFactoryDestroyObject( _object );
			}

			this->_destroyObject( _object );

			--m_count;
			intrusive_ptr_dec_ref( this );
		}
		else
		{
			m_mutex->lock();

			if( m_destroyListener != nullptr )
			{
				m_destroyListener->onFactoryDestroyObject( _object );
			}

			this->_destroyObject( _object );

			--m_count;
			intrusive_ptr_dec_ref( this );

			m_mutex->unlock();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool Factory::emptyObject() const
    {
        return m_count == 0;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t Factory::countObject() const
	{
		return m_count;
	}
    //////////////////////////////////////////////////////////////////////////
    void Factory::_destroy()
    {
        FACTORY_SERVICE( m_serviceProvider )
            ->unregisterFactory( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroy()
    {
        this->_destroy();

        delete this;
    }
}
