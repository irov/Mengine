#include "FactoryService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/CRC32.h"
#include "Kernel/Map.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FactoryService, Mengine::FactoryService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryService::FactoryService()
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        : m_generation( 0 )
        , m_memleakDetection( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryService::~FactoryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryService::_initializeService()
    {
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        m_memleakDetection = HAS_OPTION( "memleak" );

        if( m_memleakDetection == true )
        {
            SERVICE_WAIT( ThreadSystemInterface, [this]()
            {
                ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
                    ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( mutex );

                m_mutex = mutex;

                return true;
            } );

            SERVICE_LEAVE( ThreadSystemInterface, [this]()
            {
                m_mutex = nullptr;
            } );
        }

        m_memleakLogFileName = GET_OPTION_VALUE( "memleaklog", "" );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::_finalizeService()
    {
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        if( m_memleakDetection == false )
        {
            return;
        }

        uint32_t leakcount = 0;

#if defined(MENGINE_DOCUMENT_ENABLE)
        typedef Vector<DocumentInterfacePtr> VectorDocuments;
        typedef Map<const FactoryInterface *, VectorDocuments> MapObjectLeaks;
        MapObjectLeaks objectLeaks;
        this->debugFactoryForeachLeakObjects( 0, [&leakcount, &objectLeaks]( const FactoryInterface * _factory, const Factorable * _factorable )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _factorable );

            const DocumentInterfacePtr & doc = _factorable->getDocument();

            if( doc == nullptr )
            {
                return;
            }

            objectLeaks[_factory].emplace_back( doc );

            ++leakcount;
        } );
#else
        this->debugFactoryForeachLeakObjects( 0, [&leakcount]( const FactoryInterface * _factory, const Factorable * _factorable )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _factorable );

            ++leakcount;
        } );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
        if( m_memleakLogFileName.empty() == false )
        {
            FILE * f = ::fopen( m_memleakLogFileName.c_str(), "wb" );

            if( f != NULL )
            {
                Char objectleakmsg[1024] = {'\0'};
                int32_t objectleakmsg_length = MENGINE_SNPRINTF( objectleakmsg, 1023,
                    "**********************************************************\n"
                    "<<<<<                OBJECT LEAK [% .7u]           >>>>>\n"
                    "**********************************************************\n"
                    , leakcount
                );

                ::fwrite( objectleakmsg, objectleakmsg_length, 1, f );

#if defined(MENGINE_DOCUMENT_ENABLE)
                for( auto && [factory, objects] : objectLeaks )
                {
                    const Char * factory_delimiter = "##########################################################\n";
                    ::fwrite( factory_delimiter, StdString::strlen( factory_delimiter ), 1, f );

                    const ConstString & factoryType = factory->getType();

                    Char factorymsg[2048] = {'\0'};
                    int32_t factorymsg_length = MENGINE_SNPRINTF( factorymsg, 2047, "Factory '%s' [%zu]:\n"
                        , factoryType.c_str()
                        , objects.size()
                    );

                    ::fwrite( factorymsg, factorymsg_length, 1, f );

                    ::fwrite( factory_delimiter, StdString::strlen( factory_delimiter ), 1, f );

                    for( const DocumentInterfacePtr & obj : objects )
                    {
                        const Char * obj_delimiter = "**********************************************************\n";
                        ::fwrite( obj_delimiter, StdString::strlen( obj_delimiter ), 1, f );

                        Char objmsg[16384] = {'\0'};
                        int32_t objmsg_length = MENGINE_SNPRINTF( objmsg, 16383, "    doc: %s\n"
                            , MENGINE_DOCUMENT_STR( obj )
                        );

                        ::fwrite( objmsg, objmsg_length, 1, f );
                    }
                }
#endif

                ::fclose( f );
            }
        }

        m_memleakLogFileName.clear();
#endif

        MENGINE_ASSERTION_FATAL( leakcount == 0, "detect factory leak [%u]"
            , leakcount
        );

        for( uint32_t index = 0; index != MENGINE_NODELEAKDETECTOR_HASHSIZE; ++index )
        {
            VectorObjectLeakDesc & nodeLeakDescs = m_objectLeakDescs[index];

            nodeLeakDescs.clear();
        }
#endif

        MENGINE_ASSERTION_FATAL( m_factories.empty() == true, "detect factory leak [%" MENGINE_PRIuPTR "]"
            , m_factories.size()
        );

        m_factories.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::registerFactory( const FactoryInterface * _factory )
    {
        FactoryDesc desc;
        desc.factory = _factory;

        m_factories.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::unregisterFactory( const FactoryInterface * _factory )
    {
        VectorFactories::iterator it_found = Algorithm::find_if( m_factories.begin(), m_factories.end(), [_factory]( const FactoryDesc & _desc )
        {
            return _desc.factory == _factory;
        } );

        if( it_found == m_factories.end() )
        {
            return;
        }

        m_factories.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::foreachFactories( const LambdaFactory & _lambda )
    {
        for( const FactoryDesc & desc : m_factories )
        {
            const FactoryInterface * factory = desc.factory;

            _lambda( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)    
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryIncrefGeneration()
    {
        ++m_generation;

        NOTIFICATION_NOTIFY( NOTIFICATOR_INCREF_FACTORY_GENERATION, m_generation );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryService::debugFactoryGetGeneration() const
    {
        return m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryForeachLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const
    {
        if( m_memleakDetection == false )
        {
            return;
        }

        for( uint32_t index = 0; index != MENGINE_NODELEAKDETECTOR_HASHSIZE; ++index )
        {
            const VectorObjectLeakDesc & nodeLeakDescs = m_objectLeakDescs[index];

            for( const ObjectLeakDesc & desc : nodeLeakDescs )
            {
                if( _generation != ~0U && desc.generation != _generation )
                {
                    continue;
                }

                _leaks( desc.factory, desc.factorable );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryCreateObject( const FactoryInterface * _factory, const Factorable * _factorable )
    {
        if( m_memleakDetection == false )
        {
            return;
        }

        if( this->isInitializeService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        uint32_t hash = Helper::makeCRC32ModPod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

        ObjectLeakDesc desc;
        desc.generation = m_generation;
        desc.factory = _factory;
        desc.factorable = _factorable;

        VectorObjectLeakDesc & descs = m_objectLeakDescs[hash];

        descs.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryDestroyObject( const FactoryInterface * _factory, const Factorable * _factorable )
    {
        if( m_memleakDetection == false )
        {
            return;
        }

        if( this->isInitializeService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        uint32_t hash = Helper::makeCRC32ModPod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

        VectorObjectLeakDesc & nodeLeakDescs = m_objectLeakDescs[hash];

        for( VectorObjectLeakDesc::iterator
            it = nodeLeakDescs.begin(),
            it_end = nodeLeakDescs.end();
            it != it_end;
            ++it )
        {
            const ObjectLeakDesc & desc = *it;

            if( desc.factory != _factory || desc.factorable != _factorable )
            {
                continue;
            }

            *it = nodeLeakDescs.back();
            nodeLeakDescs.pop_back();

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}