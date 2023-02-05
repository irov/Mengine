#include "MockupHttpSystem.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "ThreadTaskDummy.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::MockupHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	MockupHttpSystem::MockupHttpSystem()
		: m_enumeratorReceivers(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MockupHttpSystem::_initializeService()
	{
		if( THREAD_SERVICE()
			->createThread( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem" ), MENGINE_THREAD_PRIORITY_LOWEST, __FILE__, __LINE__ ) == false )
		{
			return false;
		}

		m_factoryTaskDummy = new FactoryPool<ThreadTaskDummy, 8>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MockupHttpSystem::_finalizeService()
	{
		THREAD_SERVICE()
			->destroyThread( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem" ) );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDummy );

		m_factoryTaskDummy = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::getMessage( const String & _url, const HttpReceiverInterfacePtr & _receiver )
	{
		uint32_t task_id = ++m_enumeratorReceivers;

		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->initialize( task_id, this );

		if( THREAD_SERVICE()
			->addTask( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR("MockupHttpSystem::getMessage url '%s' invalid add task"
				, _url.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.emplace_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::postMessage( const String & _url, const TMapParams & _params, const HttpReceiverInterfacePtr & _receiver )
	{
		(void)_params;

		uint32_t task_id = ++m_enumeratorReceivers;

		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->initialize( task_id, this );

		if( THREAD_SERVICE()
			->addTask( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR("MockupHttpSystem::postMessage url '%s' invalid add task"
				, _url.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.emplace_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID MockupHttpSystem::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _category, const FilePath & _path, const HttpReceiverInterfacePtr & _receiver )
	{
		(void)_login;
		(void)_password;

		if( _category->existFile( _path ) == true )
		{
			LOGGER_ERROR("MockupHttpSystem::downloadAsset url '%s' category '%s' file already exist '%s'"
				, _url.c_str()
				, _category->getName().c_str()
				, _path.c_str()
				);

			return 0;
		}

		uint32_t task_id = ++m_enumeratorReceivers;
		
		ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();

		task->initialize( task_id, this );
		
		if( THREAD_SERVICE()
			->addTask( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem"), task ) == false )
		{
			LOGGER_ERROR("MockupHttpSystem::downloadAsset url '%s' category '%s' path '%s' invalid add task"
				, _url.c_str()
				, _category->getName().c_str()
				, _path.c_str()
				);

			return 0;
		}

		HttpReceiverDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_receiverDescs.emplace_back( desc );

		return task_id;
	}
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID MockupHttpSystem::headerData( const String & _url, const TVectorString & _headers, const String & _data, const HttpReceiverInterfacePtr & _receiver )
    {
        (void)_url;
        (void)_headers;
        (void)_data;
        
        uint32_t task_id = ++m_enumeratorReceivers;
        
        ThreadTaskDummyPtr task = m_factoryTaskDummy->createObject();
        
        task->initialize( task_id, this );
        
        if( THREAD_SERVICE()
           ->addTask( STRINGIZE_STRING_LOCAL( "ThreadMockupHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR("MockupHttpSystem::headerData url '%s' invalid add task"
                                              , _url.c_str()
                                              );
            
            return 0;
        }
        
        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;
        
        m_receiverDescs.emplace_back( desc );
        
        return task_id;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MockupHttpSystem::cancelRequest( HttpRequestID _id )
	{
        for( HttpReceiverDesc & desc : m_receiverDescs )
		{
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

            HttpReceiverInterfacePtr receiver = desc.receiver;

			m_receiverDescs.erase( it );

			if( receiver != nullptr)
			{
				receiver->onHttpRequestComplete( _id, _status, _response, _code, _successful );
			}
			
			break;
		}
	}
}
