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

        MENGINE_ASSERTION_CRITICAL( leakcount == 0, "detect factory leak [%d]"
            , leakcount
        );
#endif

        MENGINE_ASSERTION_CRITICAL( m_factories.empty() == true, "detect factory leak [%d]"
            , m_factories.size()
        );
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