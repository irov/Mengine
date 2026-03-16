#include "PythonSceneEventReceiver.h"

#include "PythonEntityBehavior.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class ... Args>
        static void callSceneBehavior( const pybind::object & _cb, const EntityBehaviorInterfacePtr & _behavior, Args && ... _args )
        {
            PythonEntityBehavior * py_behavior = _behavior.getT<PythonEntityBehavior *>();

            py_behavior->call( _cb, std::forward<Args>( _args ) ... );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PythonSceneEventReceiver::PythonSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonSceneEventReceiver::~PythonSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        Detail::callSceneBehavior( m_cb, _behavior, _focus );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onSceneRestartBegin( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onSceneRestartEnd( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        Detail::callSceneBehavior( m_cb, _behavior, _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callSceneBehavior( m_cb, _behavior );
    }
}