#include "FactoryService.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/CRC32.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FactoryService, Mengine::FactoryService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryService::FactoryService()
#ifdef MENGINE_DEBUG
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
#ifdef MENGINE_DEBUG
        m_memleakDetection = HAS_OPTION( "memleak" );

        m_memleakLogFileName = GET_OPTION_VALUE( "memleaklog", "" );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::_finalizeService()
    {
#ifdef MENGINE_DEBUG
        VectorString factoryLeaks;
        for( const FactoryDesc & desc : m_factories )
        {
            factoryLeaks.emplace_back( desc.factory_name );
        }
#endif

#ifdef MENGINE_DEBUG
        if( m_memleakDetection == false )
        {
            return;
        }

        uint32_t leakcount = 0;

        typedef Map<String, VectorString> MapObjectLeaks;
        MapObjectLeaks objectLeaks;
        this->visitFactoryLeakObjects( 0, [&leakcount, &objectLeaks]( const Factory * _factory, const Factorable * _factorable, const Char * _name, const Char * _doc )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _factorable );


            objectLeaks[_name].emplace_back( _doc );

            ++leakcount;
        } );

#ifdef MENGINE_PLATFORM_WINDOWS
        if( m_memleakLogFileName.empty() == false )
        {
            FILE * f = fopen( m_memleakLogFileName.c_str(), "wb" );

            if( f != NULL )
            {
                for( auto && [name, objects] : objectLeaks )
                {
                    const Char * factory_delimiter = "##########################################################\n";
                    fwrite( factory_delimiter, strlen( factory_delimiter ), 1, f );

                    Char factorymsg[2048];
                    int factorymsg_length = snprintf( factorymsg, 2048, "Factory '%s' [%u]:\n"
                        , name.c_str()
                        , objects.size()
                    );

                    fwrite( factorymsg, factorymsg_length, 1, f );

                    for( const String & obj : objects )
                    {
                        const Char * obj_delimiter = "**********************************************************\n";
                        fwrite( obj_delimiter, strlen( obj_delimiter ), 1, f );

                        Char objmsg[16384];
                        int objmsg_length = snprintf( objmsg, 16384, "    doc: %s\n"
                            , obj.c_str()
                        );

                        fwrite( objmsg, objmsg_length, 1, f );
                    }

                    fwrite( factory_delimiter, strlen( factory_delimiter ), 1, f );
                }

                Char finalmsg[1024];
                int finalmsg_length = sprintf( finalmsg
                    , "**********************************************************\n"
                      "<<<<<                OBJECT LEAK [%.7u]           >>>>>\n"
                      "**********************************************************\n"
                    , leakcount
                );

                fwrite( finalmsg, finalmsg_length, 1, f );

                fclose( f );
            }
        }
#endif

        MENGINE_ASSERTION_CRITICAL( leakcount == 0, "detect factory leak [%d]"
            , leakcount
        );

        for( uint32_t index = 0; index != MENGINE_NODELEAKDETECTOR_HASHSIZE; ++index )
        {
            VectorObjectLeakDesc & nodeLeakDescs = m_objectLeakDescs[index];

            nodeLeakDescs.clear();
        }



#endif

        MENGINE_ASSERTION_CRITICAL( m_factories.empty() == true, "detect factory leak [%d]"
            , m_factories.size()
        );

        m_factories.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::registerFactory( const Factory * _factory )
    {
        FactoryDesc desc;
        desc.factory = _factory;

#ifdef MENGINE_DEBUG
        desc.factory_name = _factory->getName();
#endif

        m_factories.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::unregisterFactory( const Factory * _factory )
    {
        VectorFactories::iterator it_found = std::find_if( m_factories.begin(), m_factories.end(), [_factory]( const FactoryDesc & _desc )
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
    void FactoryService::visitFactories( const VisitorFactoryServicePtr & _visitor )
    {
        for( const FactoryDesc & desc : m_factories )
        {
            const Factory * factory = desc.factory;

            _visitor->visit( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::increfFactoryGeneration()
    {
#ifdef MENGINE_DEBUG
        ++m_generation;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryService::getFactoryGeneration() const
    {
#ifdef MENGINE_DEBUG
        return m_generation;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::visitFactoryLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const
    {
#ifdef MENGINE_DEBUG
        for( uint32_t index = 0; index != MENGINE_NODELEAKDETECTOR_HASHSIZE; ++index )
        {
            const VectorObjectLeakDesc & nodeLeakDescs = m_objectLeakDescs[index];

            for( const ObjectLeakDesc & desc : nodeLeakDescs )
            {
                if( desc.generation != _generation )
                {
                    continue;
                }

                _leaks( desc.factory, desc.factorable, desc.factory_name.c_str(), desc.factorable_doc.c_str() );
            }
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryCreateObject( const Factory * _factory, const Factorable * _factorable, const Char * _doc )
    {
#ifdef MENGINE_DEBUG
        if( m_memleakDetection == false )
        {
            return;
        }

        uint32_t hash = Helper::make_crc32_mod_pod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

        ObjectLeakDesc desc;
        desc.generation = m_generation;
        desc.factory = _factory;
        desc.factorable = _factorable;
        desc.factory_name = _factory->getName();
        desc.factorable_doc = _doc;

        VectorObjectLeakDesc & descs = m_objectLeakDescs[hash];

        descs.emplace_back( desc );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryService::debugFactoryDestroyObject( const Factory * _factory, const Factorable * _factorable )
    {
#ifdef MENGINE_DEBUG
        if( m_memleakDetection == false )
        {
            return;
        }

        uint32_t hash = Helper::make_crc32_mod_pod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

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
#endif
    }
}