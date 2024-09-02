#include "Box2DService.h"

#include "Box2DWorld.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Box2DService, Mengine::Box2DService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DService::Box2DService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DService::~Box2DService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DService::_initializeService()
    {
        m_factoryWorlds = Helper::makeFactoryPool<Box2DWorld, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWorlds );

        m_factoryWorlds = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DWorldInterfacePtr Box2DService::createWorld( const mt::vec2f & _gravity, float _scaler, const DocumentInterfacePtr & _doc )
    {
        Box2DWorldPtr world = m_factoryWorlds->createObject( _doc );

        if( world->initialize( _gravity, _scaler ) == false )
        {
            return nullptr;
        }

        return world;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DService::destroyWorld( const Box2DWorldInterfacePtr & _world )
    {
        Box2DWorldPtr world = Box2DWorldPtr::from( _world );
        
        world->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}