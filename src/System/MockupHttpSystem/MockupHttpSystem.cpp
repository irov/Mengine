#	include "MockupHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskDummy.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Menge::MockupHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MockupHttpSystem::MockupHttpSystem()
		: m_enumeratorReceivers(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MockupHttpSystem::_initialize()
	{
		if( THREAD_SERVICE( m_serviceProvider )
			->createThread( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadMockupHttpSystem" ), MENGINE_THREAD_PRIORITY_LOWEST, __FILE__, __LINE__ ) == false )
		{
			return false;
		}

		m_factoryTaskDummy = new FactoryPool<ThreadTaskDummy, 8>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MockupHttpSystem::_finalize()
	{
		THREAD_SERVICE( m_serviceProvider )
			->destroyThread( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadMockupHttpSystem" ) );

		m_factoryTaskDummy = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::getMessage( const String & _url, HttpReceiver * _receiver )
	{
		uint32_t task_id = ++m_enumeratorReceivers;

		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( task_id, this );

		if( THREAD_SERVICE( m_serviceProvider )
			->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadMockupHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MockupHttpSystem::getMessage url '%s' invalid add task"
				, _url.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.push_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::postMessage( const String & _url, const TMapParams & _params, HttpReceiver * _receiver )
	{
		(void)_params;

		uint32_t task_id = ++m_enumeratorReceivers;

		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( task_id, this );

		if( THREAD_SERVICE( m_serviceProvider )
			->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadMockupHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MockupHttpSystem::postMessage url '%s' invalid add task"
				, _url.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.push_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _path, HttpReceiver * _receiver )
	{
		(void)_login;
		(void)_password;

		if( FILE_SERVICE( m_serviceProvider )
			->hasFileGroup( _category, nullptr ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MockupHttpSystem::downloadAsset url '%s' not found category '%s' for filepath '%s'"
				, _url.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		if( FILE_SERVICE( m_serviceProvider )
			->existFile( _category, _path, nullptr ) == true )
		{
			LOGGER_ERROR( m_serviceProvider )("MockupHttpSystem::downloadAsset url '%s' category '%s' file alredy exist '%s'"
				, _url.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		uint32_t task_id = ++m_enumeratorReceivers;
		
		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( task_id, this );
		
		if( THREAD_SERVICE(m_serviceProvider)
			->addTask( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadMockupHttpSystem"), task ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MockupHttpSystem::downloadAsset url '%s' category '%s' path '%s' invalid add task"
				, _url.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.push_back( desc );

		return task_id;
	}
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID MockupHttpSystem::headerData( const String & _url, const TVectorString & _headers, const String & _data, HttpReceiver * _receiver )
    {
        (void)_url;
        (void)_headers;
        (void)_data;
        
        uint32_t task_id = ++m_enumeratorReceivers;
        
        ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();
        
        task->setServiceProvider( m_serviceProvider );
        task->initialize( task_id, this );
        
        if( THREAD_SERVICE( m_serviceProvider )
           ->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadMockupHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("MockupHttpSystem::headerData url '%s' invalid add task"
                                              , _url.c_str()
                                              );
            
            return 0;
        }
        
        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;
        
        m_receiverDescs.push_back( desc );
        
        return task_id;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MockupHttpSystem::cancelRequest( HttpRequestID _id )
	{
		for( TVectorHttpReceiverDesc::iterator
			it = m_receiverDescs.begin(),
			it_end = m_receiverDescs.end();
			it != it_end;
			++it )
		{
			HttpReceiverDesc & desc = *it;

			if( desc.id != _id )
			{
				continue;
			}

			desc.task->cancel();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MockupHttpSystem::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _response, uint32_t _code, bool _successful )
	{
		for( TVectorHttpReceiverDesc::iterator
			it = m_receiverDescs.begin(),
			it_end = m_receiverDescs.end();
		it != it_end;
		++it )
		{
			HttpReceiverDesc & desc = *it;

			if( desc.id != _id )
			{
				continue;
			}

			HttpReceiver * receiver = desc.receiver;

			m_receiverDescs.erase( it );

			if( receiver != nullptr)
			{
				receiver->onHttpRequestComplete( _id, _status, _response, _code, _successful );
			}
			
			break;
		}
	}
}
