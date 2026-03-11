#include "PythonLayerEventReceiver.h"

#include "PythonEntityBehavior.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class ... Args>
        static void callLayerBehavior( const pybind::object & _cb, const EntityBehaviorInterfacePtr & _behavior, Args && ... _args )
        {
            PythonEntityBehavior * py_behavior = _behavior.getT<PythonEntityBehavior *>();

            py_behavior->call( _cb, std::forward<Args>( _args ) ... );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PythonLayerEventReceiver::PythonLayerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonLayerEventReceiver::~PythonLayerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayerEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayerEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonLayerEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonLayerEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonLayerEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonLayerEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayerEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        Detail::callLayerBehavior( m_cb, _behavior, _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonLayerEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callLayerBehavior( m_cb, _behavior );
    }
}