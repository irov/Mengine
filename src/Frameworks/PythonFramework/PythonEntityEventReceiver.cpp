#include "PythonEntityEventReceiver.h"

#include "PythonEntityBehavior.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class ... Args>
        void callBehavior( const pybind::object & _cb, const EntityBehaviorInterfacePtr & _behavior, Args ... _args )
        {
            PythonEntityBehavior * py_behavior = _behavior.getT<PythonEntityBehavior *>();

            py_behavior->call( _cb, _args ... );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PythonEntityEventReceiver::PythonEntityEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonEntityEventReceiver::~PythonEntityEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        Detail::callBehavior( m_cb, _behavior, _entity );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonEntityEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        Detail::callBehavior( m_cb, _behavior );
    }
}